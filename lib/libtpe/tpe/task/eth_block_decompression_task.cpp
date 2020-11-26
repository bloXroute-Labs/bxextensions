#include <utility>

#include "tpe/task/eth_block_decompression_task.h"

#include <utils/common/buffer_helper.h>
#include <utils/encoding/rlp_encoder.h>
#include <utils/common/string_helper.h>


namespace task {

EthBlockDecompressionTask::EthBlockDecompressionTask(
		size_t capacity/* = ETH_DEFAULT_BLOCK_SIZE*/,
		size_t minimal_tx_count/* = ETH_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/
) :
        _block_hash(nullptr),
		_tx_service(nullptr),
		_minimal_tx_count(minimal_tx_count),
		_success(false),
        _txn_count(0),
        _content_size(0),
        _starting_offset(0)
{
    _block_buffer = std::make_shared<BlockBuffer_t>(BlockBuffer_t::empty());
	_output_buffer = std::make_shared<ByteArray_t>(capacity);
}

void EthBlockDecompressionTask::init(
        PBlockBuffer_t block_buffer,
		PTransactionService_t tx_service
)
{
	_unknown_tx_hashes.clear();
	if (_output_buffer.use_count() > 1) {
		_output_buffer = std::make_shared<ByteArray_t>(_output_buffer->capacity());
	} else {
		_output_buffer->reserve(_output_buffer->capacity());
		_output_buffer->reset();
	}
	_unknown_tx_sids.clear();
    _block_buffer = std::move(block_buffer);
    _block_hash = nullptr;
    _tx_service = std::move(tx_service);
	_short_ids.clear();
    _txn_count = 0;
    _content_size = 0;
    _starting_offset = 0;
}

PByteArray_t EthBlockDecompressionTask::eth_block() {
	assert_execution();
	return _output_buffer;
}

const UnknownTxHashes_t& EthBlockDecompressionTask::unknown_tx_hashes() {
	assert_execution();
	return _unknown_tx_hashes;
}

const UnknownTxSIDs_t& EthBlockDecompressionTask::unknown_tx_sids() {
	assert_execution();
	return _unknown_tx_sids;
}

PSha256_t EthBlockDecompressionTask::block_hash() {
	assert_execution();
	return _block_hash;
}

bool EthBlockDecompressionTask::success() {
	assert_execution();
	return _success;
}

uint64_t EthBlockDecompressionTask::tx_count() {
	assert_execution();
	return _txn_count;
}

const std::vector<unsigned int>& EthBlockDecompressionTask::short_ids() {
	assert_execution();
	return _short_ids;
}

size_t EthBlockDecompressionTask::starting_offset() {
    return _starting_offset;
}

size_t EthBlockDecompressionTask::get_task_byte_size() const {
    size_t sub_tasks_size = 0;
    for (const auto& p_task: _sub_tasks) {
        sub_tasks_size += (
            sizeof(p_task) + sizeof(EthBlockDecompressionSubTask) + sizeof(EthTXOffsets_t) +
            p_task->task_data().offsets->size() * (2 * sizeof(size_t))
        );
    }
    size_t block_buffer_size = 0;
    if (_block_buffer != nullptr) {
        block_buffer_size = _block_buffer->size();
    }
    return sizeof(EthBlockDecompressionTask) + _output_buffer->capacity() + block_buffer_size + sub_tasks_size;
}

void EthBlockDecompressionTask::cleanup() {
    assert_execution();
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void EthBlockDecompressionTask::_execute(SubPool_t& sub_pool) {
    BxEthBlockMessage_t msg(*_block_buffer);
    msg.parse();
    msg.deserialize_short_ids(_short_ids);
    _block_header = BlockBuffer_t(msg.block_header());
    _block_trailer = BlockBuffer_t(msg.block_trailer());
    _block_hash = std::make_shared<Sha256_t>(std::move(msg.block_hash()));
    _success = _tx_service->get_missing_transactions(_unknown_tx_hashes, _unknown_tx_sids, _short_ids);
    if ( ! _success) {
        return;
    }

    size_t txn_offset = msg.txn_offset();
    size_t txn_end_offset = msg.txn_end_offset();
    size_t output_offset = _dispatch(txn_end_offset, msg, txn_offset, sub_pool);

    for (auto& task: _sub_tasks) {
        task->wait();
    }
    _set_output_buffer(output_offset);
}

void EthBlockDecompressionTask::_init_sub_tasks(size_t pool_size)
{
    if (_sub_tasks.size() < pool_size) {
        for (size_t i = _sub_tasks.size() ; i < pool_size ; ++i) {
            _sub_tasks.push_back(std::make_shared<EthBlockDecompressionSubTask>());
        }
    }
    for (auto& task_data : _sub_tasks) {
        task_data->task_data().clear();
    }
}

size_t EthBlockDecompressionTask::_dispatch(
    size_t txn_end_offset, BxEthBlockMessage_t& msg, size_t offset, SubPool_t& sub_pool
)
{
    size_t idx = 0, pool_size = sub_pool.size(), prev_idx = 0, short_ids_offset = 0;
    _init_sub_tasks(pool_size);
    size_t bulk_size = std::max((size_t) (ETH_DEFAULT_TX_COUNT / pool_size), std::max(pool_size, _minimal_tx_count));
    size_t output_offset = sizeof(uint64_t) + sizeof(uint64_t) + _block_header.size();
    _extend_output_buffer(output_offset);
    size_t prev_output_offset = output_offset;
    bool is_short;

    while (offset < txn_end_offset) {
        size_t tx_content_offset;
        uint64_t tx_content_len = 0;
        auto& tdata = _sub_tasks[idx]->task_data();
        idx = std::min((size_t) (_txn_count / bulk_size), pool_size - 1);

        offset = msg.get_next_tx_offset(offset, tx_content_offset, is_short);

        if ( is_short ) {
            const size_t short_id_idx = short_ids_offset + tdata.short_ids_len;
            if ( short_id_idx < _short_ids.size()) {
                unsigned int short_id = _short_ids.at(short_id_idx);
                tdata.short_ids_len += 1;
                output_offset += _tx_service->get_tx_size(short_id);
            } else {
                throw std::runtime_error(utils::common::concatenate(
                        "Message is improperly formatted, short id index (",
                        short_id_idx,
                        ") exceeded its array bounds (size: ",
                        _short_ids.size(),
                        ")"
                ));  // TODO: throw proper exception here
            }
        } else {
            ++tx_content_offset;
            tx_content_offset = utils::encoding::consume_length_prefix(
                *_block_buffer, tx_content_len, tx_content_offset
            );
            output_offset += tx_content_len;
        }
        tdata.offsets->push_back(std::make_tuple(tx_content_offset, (size_t)is_short, tx_content_len));
        if(idx > prev_idx) {
            tdata.output_offset = prev_output_offset;
            tdata.short_ids_offset = short_ids_offset;
            _extend_output_buffer(output_offset);
            _enqueue_task(prev_idx, sub_pool);
            short_ids_offset += tdata.short_ids_len;
            prev_output_offset = output_offset;
        }
        prev_idx = idx;
        ++_txn_count;
    }

    _sub_tasks[prev_idx]->task_data().output_offset = prev_output_offset;
    _sub_tasks[prev_idx]->task_data().short_ids_offset = short_ids_offset;
    _enqueue_task(prev_idx, sub_pool);
    return output_offset;
}

void EthBlockDecompressionTask::_enqueue_task(size_t task_idx, SubPool_t& sub_pool)
{
    auto task = _sub_tasks[task_idx];
    task->init(
        _tx_service,
        _block_buffer.get(),
        _output_buffer.get(),
        task->task_data().offsets,
        &_short_ids
    );
    sub_pool.enqueue_task(task);
}

void EthBlockDecompressionTask::_set_output_buffer(size_t output_offset)
{
    if ( _unknown_tx_sids.size() == 0 && _unknown_tx_hashes.size() == 0) {
        size_t tx_origin_offset = sizeof(size_t) + sizeof(size_t) + _block_header.size();
        // handling content len
        _content_size = output_offset - tx_origin_offset;
        ByteArray_t list_of_txs_prefix_buffer = ByteArray_t();
        utils::encoding::get_length_prefix_list(list_of_txs_prefix_buffer, _content_size, 0);
        tx_origin_offset -= list_of_txs_prefix_buffer.size();
        _output_buffer->copy_from_array(
            list_of_txs_prefix_buffer.array(), tx_origin_offset, 0, list_of_txs_prefix_buffer.size()
        );

        //  handling block trailer
        if (_block_trailer.size() > 0) {
            _output_buffer->resize(output_offset);
            _output_buffer->operator +=(_block_trailer);
            output_offset += _block_trailer.size();
        }
        _content_size = output_offset - tx_origin_offset;

        // handling block header
        tx_origin_offset -= _block_header.size();
        _content_size += _block_header.size();
        _output_buffer->copy_from_buffer(_block_header, tx_origin_offset, 0, _block_header.size());

        // handling msg len
        ByteArray_t block_msg_prefix_buf = ByteArray_t();
        utils::encoding::get_length_prefix_list(block_msg_prefix_buf, _content_size, 0);
        tx_origin_offset -= block_msg_prefix_buf.size();
        _output_buffer->copy_from_array(
            block_msg_prefix_buf.array(), tx_origin_offset, 0, block_msg_prefix_buf.size()
        );

        _output_buffer->resize(output_offset);
        _starting_offset = tx_origin_offset;
        _output_buffer->set_output();
    }
}

void EthBlockDecompressionTask::_extend_output_buffer(size_t output_offset)
{
    if (output_offset > _output_buffer->capacity()) {
        std::string error = utils::common::concatenate(
            "not enough space allocated to output buffer. \ncapacity - ",
            _output_buffer->capacity(),
            ", required size - ",
            output_offset
        );
        throw std::runtime_error(error);
    }
    _output_buffer->resize(output_offset);
}

} // task
