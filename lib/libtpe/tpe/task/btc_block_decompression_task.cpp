#include <utility>

#include "tpe/task/btc_block_decompression_task.h"

#include <utils/common/buffer_helper.h>
#include <utils/common/string_helper.h>


namespace task {

BtcBlockDecompressionTask::BtcBlockDecompressionTask(
		size_t capacity/* = BTC_DEFAULT_BLOCK_SIZE*/,
		size_t minimal_tx_count/* = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/
) :
		_block_hash(nullptr),
		_tx_service(nullptr),
		_minimal_tx_count(minimal_tx_count),
		_success(false),
		_tx_count(0)
{
    _block_buffer = std::make_shared<BlockBuffer_t>(BlockBuffer_t::empty());
	_output_buffer = std::make_shared<ByteArray_t>(capacity);
}

void BtcBlockDecompressionTask::init(
        PBlockBuffer_t block_buffer,
		PTransactionService_t tx_service
)
{
	_unknown_tx_hashes.clear();
	const uint32_t output_size = std::max(
			(size_t)BxBtcBlockMessage_t::get_original_block_size(
					*block_buffer
			),
			_output_buffer->capacity()
	);
	if (_output_buffer.use_count() > 1) {
		_output_buffer =  std::make_shared<ByteArray_t>(
				output_size
		);
	} else {
		_output_buffer->reserve(output_size);
		_output_buffer->reset();
	}
	_unknown_tx_sids.clear();
	_block_buffer = std::move(block_buffer);
	_block_hash = nullptr;
	_tx_service = std::move(tx_service);
	_short_ids.clear();
	_tx_count = 0;
}

PByteArray_t
BtcBlockDecompressionTask::btc_block() {
	assert_execution();
	return _output_buffer;
}

const UnknownTxHashes_t& BtcBlockDecompressionTask::unknown_tx_hashes() {
	assert_execution();
	return _unknown_tx_hashes;
}

const UnknownTxSIDs_t& BtcBlockDecompressionTask::unknown_tx_sids() {
	assert_execution();
	return _unknown_tx_sids;
}

PSha256_t BtcBlockDecompressionTask::block_hash() {
	assert_execution();
	return _block_hash;
}

bool BtcBlockDecompressionTask::success() {
	assert_execution();
	return _success;
}

uint64_t BtcBlockDecompressionTask::tx_count() {
	assert_execution();
	return _tx_count;
}

const std::vector<unsigned int>&
BtcBlockDecompressionTask::short_ids() {
	assert_execution();
	return _short_ids;
}

size_t BtcBlockDecompressionTask::get_task_byte_size() const {
    size_t sub_tasks_size = 0;
    for (const auto& p_task: _sub_tasks) {
        sub_tasks_size +=
                (sizeof(p_task) + sizeof(BtcBlockDecompressionSubTask) + sizeof(TXOffsets_t) +
                    p_task->task_data().offsets->size() * (2 * sizeof(size_t)));
    }
    size_t block_buffer_size = 0;
    if (_block_buffer != nullptr) {
        block_buffer_size = _block_buffer->size();
    }
    return sizeof(BtcBlockDecompressionTask) + _output_buffer->capacity() + block_buffer_size + sub_tasks_size;
}

void BtcBlockDecompressionTask::cleanup() {
    assert_execution();
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void BtcBlockDecompressionTask::_execute(SubPool_t& sub_pool) {
	size_t offset;
	BxBtcBlockMessage_t msg = _parse_block_header(offset, _tx_count);
	_success = _tx_service->get_missing_transactions(
        _unknown_tx_hashes, _unknown_tx_sids, _short_ids
	);
    _block_hash = std::make_shared<Sha256_t>(msg.block_message().block_hash());
    if (!_success) {
        return;
	}
	size_t last_idx = _dispatch(msg, offset, sub_pool);

    offset = _output_buffer->copy_from_buffer(
			*_block_buffer,
			0,
            BxBtcBlockMessage_t::offset_diff,
			offset - BxBtcBlockMessage_t::offset_diff
	);
    for (size_t idx = 0 ; idx <= last_idx ; ++idx) {
		auto& task = _sub_tasks[idx];
		task->wait();
    }
	_output_buffer->set_output();
}

void BtcBlockDecompressionTask::_init_sub_tasks(
		size_t pool_size
)
{
	size_t sub_tasks_size = _sub_tasks.size();
	if (sub_tasks_size < pool_size) {
		for (
				size_t i = sub_tasks_size ;
				i < pool_size ;
				++i
		)
		{
			_sub_tasks.push_back(
					std::make_shared<BtcBlockDecompressionSubTask>()
			);
		}
	}
	for (size_t idx = 0 ; idx < sub_tasks_size ; ++idx) {
		auto& sub_task = _sub_tasks[idx];
		sub_task->task_data().clear();
	}
}

size_t BtcBlockDecompressionTask::_dispatch(
		BxBtcBlockMessage_t& msg,
		size_t offset,
		SubPool_t& sub_pool
)
{
	size_t pool_size = sub_pool.size(), prev_idx = 0;
	_init_sub_tasks(pool_size);
	size_t bulk_size = std::max((size_t) (_tx_count / pool_size), std::max(pool_size, _minimal_tx_count));
	size_t idx = 0;
	size_t short_ids_offset = 0;
	size_t output_offset = offset - BxBtcBlockMessage_t::offset_diff;
	size_t prev_output_offset = output_offset;
	bool is_short;
	for (uint64_t count = 0 ; count < _tx_count ; ++count) {
		size_t from = offset;
		auto& tdata = _sub_tasks[idx]->task_data();
		idx = std::min((size_t) (count / bulk_size), pool_size - 1);
		size_t witness_offset;
		offset = msg.get_next_tx_offset(offset, is_short, witness_offset);
		if (is_short) {
		    const size_t short_id_idx = short_ids_offset + tdata.short_ids_len;
		    if (short_id_idx < _short_ids.size()) {
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
			output_offset += (offset - from);
		}
		tdata.offsets->push_back(std::make_tuple(from, witness_offset, offset));
		if(idx > prev_idx) {
			tdata.output_offset = prev_output_offset;
			tdata.short_ids_offset = short_ids_offset;
			_extend_output_buffer(output_offset);
			_enqueue_task(prev_idx, sub_pool);
			short_ids_offset += tdata.short_ids_len;
			prev_output_offset = output_offset;
		}
		prev_idx = idx;
	}
	_sub_tasks[prev_idx]->task_data().output_offset = prev_output_offset;
	_sub_tasks[prev_idx]->task_data().short_ids_offset = short_ids_offset;
	_extend_output_buffer(output_offset);
	_enqueue_task(prev_idx, sub_pool);
	return prev_idx;
}

void BtcBlockDecompressionTask::_enqueue_task(
		size_t task_idx,
		SubPool_t& sub_pool
)
{
	auto task = _sub_tasks[task_idx];
	task->init(
			_tx_service,
			_block_buffer.get(),
			_output_buffer.get(),
			&_short_ids
	);
	sub_pool.enqueue_task(task);
}

BxBtcBlockMessage_t
BtcBlockDecompressionTask::_parse_block_header(
		size_t& offset,
		uint64_t& tx_count
)
{
	uint64_t short_ids_offset;
	offset = utils::common::get_little_endian_value<uint64_t>(
			*_block_buffer, short_ids_offset, 0
	);
	BxBtcBlockMessage_t msg(
			*_block_buffer, short_ids_offset
	);
	offset = msg.get_tx_count(tx_count);
	msg.deserialize_short_ids(_short_ids);
	return msg;
}

void BtcBlockDecompressionTask::_extend_output_buffer(
		size_t output_offset
)
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
