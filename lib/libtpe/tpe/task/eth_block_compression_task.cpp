#include <utility>
#include <algorithm>

#include <utils/common/buffer_helper.h>
#include <utils/crypto/hash_helper.h>
#include <utils/encoding/rlp_encoder.h>
#include "tpe/task/eth_block_compression_task.h"

namespace task {

EthBlockCompressionTask::EthBlockCompressionTask(
        size_t capacity/* = ETH_DEFAULT_BLOCK_SIZE*/,
        size_t minimal_tx_count/* = ETH_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/
):
        MainTaskBase(),
        _tx_service(nullptr),
        _minimal_tx_count(minimal_tx_count),
        _txn_count(0),
        _content_size(0)
{
    _block_buffer = std::make_shared<BlockBuffer_t>(BlockBuffer_t::empty());
    _output_buffer = std::make_shared<ByteArray_t>(capacity);
}

void EthBlockCompressionTask::init(
        PBlockBuffer_t block_buffer,
        PTransactionService_t tx_service
)
{
    _tx_service = std::move(tx_service);
    _block_buffer = std::move(block_buffer);
    if (_output_buffer.use_count() > 1) {
        _output_buffer =  std::make_shared<ByteArray_t>(
                _block_buffer->size()
        );
    } else {
        _output_buffer->reserve(_block_buffer->size());
        _output_buffer->reset();
    }
    _short_ids.clear();
    _block_hash = _prev_block_hash = _compressed_block_hash = nullptr;
    _txn_count = 0;
    _content_size = 0;
}

PByteArray_t
EthBlockCompressionTask::bx_block() {
    assert_execution();
    return _output_buffer;
}

PSha256_t
EthBlockCompressionTask::prev_block_hash() {
    assert_execution();
    return _prev_block_hash;
}

PSha256_t
EthBlockCompressionTask::block_hash() {
    assert_execution();
    return _block_hash;
}

PSha256_t
EthBlockCompressionTask::compressed_block_hash() {
    assert_execution();
    return _compressed_block_hash;
}

size_t EthBlockCompressionTask::txn_count() {
    return _txn_count;
}

size_t EthBlockCompressionTask::content_size() const {
    return _content_size;
}

const std::vector<unsigned int>&
EthBlockCompressionTask::short_ids() {
    assert_execution();
    return _short_ids;
}

size_t EthBlockCompressionTask::get_task_byte_size() const {
    size_t sub_tasks_size = 0;
    for (const TaskData& sub_task_data: _sub_tasks) {
        const PSubTask_t& p_task = sub_task_data.sub_task;
        sub_tasks_size += (p_task->output_buffer().capacity() + sizeof(p_task) + sizeof(EthBlockCompressionSubTask));
    }
    size_t block_buffer_size = 0;
    if (_block_buffer != nullptr) {
        block_buffer_size = _block_buffer->size();
    }
    return sizeof(EthBlockCompressionTask) + _output_buffer->capacity() + block_buffer_size +
           (_short_ids.capacity() * sizeof(uint32_t)) + sub_tasks_size;
}

void EthBlockCompressionTask::cleanup() {
    assert_execution();
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void EthBlockCompressionTask::_execute(SubPool_t& sub_pool) {
    utils::protocols::ethereum::EthBlockMessage msg(*_block_buffer);
    msg.parse();
    _block_header = BlockBuffer_t(msg.block_header());
    _block_trailer = BlockBuffer_t(msg.block_trailer());
    _prev_block_hash = std::make_shared<Sha256_t>(std::move(msg.prev_block_hash()));
    _block_hash = std::make_shared<Sha256_t>(std::move(msg.block_hash()));

    size_t txn_offset = msg.txn_offset();
    size_t txn_end_offset = msg.txn_end_offset();
    size_t last_idx = _dispatch(txn_end_offset, msg, txn_offset, sub_pool);
    for (size_t idx = 0 ; idx <= last_idx ; ++idx) {
        TaskData& data = _sub_tasks.at(idx);
        data.sub_task->wait();
        _on_sub_task_completed(*data.sub_task);
    }
    _set_output_buffer(last_idx);

    _compressed_block_hash = std::make_shared<Sha256_t>(
        std::move(
            utils::crypto::double_sha256(
                _output_buffer->array(),
                0,
                _output_buffer->size()
            )
        )
    );
}

void EthBlockCompressionTask::_init_sub_tasks(size_t pool_size)
{
    if (_sub_tasks.size() < pool_size) {
        size_t default_count = ETH_DEFAULT_TX_COUNT;
        size_t capacity = ETH_DEFAULT_TX_SIZE * default_count;
        for (size_t i = _sub_tasks.size() ; i < pool_size ; ++i) {
            TaskData task_data;
            task_data.sub_task = std::make_shared<EthBlockCompressionSubTask>(capacity);
            task_data.offsets = std::make_shared<EthTXOffsets_t>();
            _sub_tasks.push_back(std::move(task_data));
        }
    } else {
        for (auto& task_data : _sub_tasks) {
            task_data.offsets->clear();
        }
    }
}

size_t EthBlockCompressionTask::_dispatch(
        size_t txn_end_offset,
        utils::protocols::ethereum::EthBlockMessage& msg,
        size_t offset,
        SubPool_t& sub_pool
)
{
    size_t pool_size = sub_pool.size(), prev_idx = 0;
    _init_sub_tasks(pool_size);
    size_t bulk_size = std::max((size_t) (ETH_DEFAULT_TX_COUNT / pool_size), std::max(pool_size, _minimal_tx_count));
    size_t idx;
    while (offset < txn_end_offset) {
        size_t from = offset, tx_content_offset;
        idx = std::min((size_t) (_txn_count / bulk_size), pool_size - 1);
        offset = msg.get_next_tx_offset(offset, tx_content_offset);
        _sub_tasks[idx].offsets->push_back(std::make_tuple(from, offset));
        if(idx > prev_idx) {
            _enqueue_task(prev_idx, sub_pool);
        }
        prev_idx = idx;
        ++_txn_count;
    }
    _enqueue_task(prev_idx, sub_pool);
    return prev_idx;
}

void EthBlockCompressionTask::_on_sub_task_completed(
        EthBlockCompressionSubTask& tsk
)
{
    auto& output_buffer = tsk.output_buffer();
    auto& short_ids = tsk.short_ids();
    _short_ids.reserve(_short_ids.size() + short_ids.size());
    _short_ids.insert(_short_ids.end(), short_ids.begin(), short_ids.end());
    _content_size += tsk.content_size();
}

void EthBlockCompressionTask::_set_output_buffer(size_t last_idx)
{
    size_t full_content_size = _content_size;

    ByteArray_t list_of_txs_prefix_buffer = ByteArray_t();
    utils::encoding::get_length_prefix_list(list_of_txs_prefix_buffer, full_content_size, 0);

    full_content_size += (list_of_txs_prefix_buffer.size() + _block_header.size() + _block_trailer.size());
    ByteArray_t compact_block_msg_prefix_buf = ByteArray_t();
    utils::encoding::get_length_prefix_list(compact_block_msg_prefix_buf, full_content_size, 0);

    full_content_size += compact_block_msg_prefix_buf.size() + sizeof(uint64_t);

    // start setting output buffer
    utils::common::set_little_endian_value(
        *_output_buffer, (uint64_t)full_content_size,0
    );

    _output_buffer->operator +=(compact_block_msg_prefix_buf);
    _output_buffer->operator +=(_block_header);
    _output_buffer->operator +=(list_of_txs_prefix_buffer);

    for (size_t idx = 0 ; idx <= last_idx ; ++idx) {
        TaskData& data = _sub_tasks.at(idx);
        _output_buffer->operator +=((*data.sub_task).output_buffer());
    }

    if (_block_trailer.size() > 0) {
       _output_buffer->operator +=(_block_trailer);
    }

    const unsigned int short_ids_size = _short_ids.size();

    size_t output_offset = utils::common::set_little_endian_value(
        *_output_buffer, short_ids_size, full_content_size
    );

    if (short_ids_size > 0) {
        const size_t short_ids_byte_size = short_ids_size * sizeof(unsigned int);
        _output_buffer->resize(output_offset + short_ids_byte_size);
        memcpy(
            &_output_buffer->at(output_offset),
            (unsigned char*) &_short_ids.at(0),
            short_ids_byte_size
        );
    }
    _output_buffer->set_output();
}

void EthBlockCompressionTask::_enqueue_task(
        size_t task_idx,
        SubPool_t& sub_pool
)
{
    TaskData& data = _sub_tasks[task_idx];
    data.sub_task->init(
        _tx_service,
        _block_buffer.get(),
        data.offsets
    );
    sub_pool.enqueue_task(data.sub_task);
}

} // task
