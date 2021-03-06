#include <utility>

#include <utils/common/buffer_helper.h>
#include <utils/common/string_helper.h>
#include <utils/crypto/hash_helper.h>
#include <algorithm>

#include "tpe/task/ont_block_compression_task.h"

namespace task {

OntBlockCompressionTask::OntBlockCompressionTask(
    size_t capacity/* = ONT_DEFAULT_BLOCK_SIZE*/,
    size_t minimal_tx_count/* = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/

):
    MainTaskBase(),
    _tx_service(nullptr),
    _minimal_tx_count(minimal_tx_count),
    _txn_count(0),
    _enable_block_compression(false),
    _min_tx_age_seconds(0.0)
{
    _block_buffer = std::make_shared<BlockBuffer_t>(BlockBuffer_t::empty());
    _output_buffer = std::make_shared<ByteArray_t>(capacity);
}

void OntBlockCompressionTask::init(
    PBlockBuffer_t block_buffer,
    PTransactionService_t tx_service,
    bool enable_block_compression,
    double min_tx_age_seconds
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
    _ignored_short_ids.clear();
    _block_hash = _prev_block_hash = _compressed_block_hash = nullptr;
    _txn_count = 0;
    _enable_block_compression = enable_block_compression;
    _min_tx_age_seconds = min_tx_age_seconds;
}

PByteArray_t
OntBlockCompressionTask::bx_block() {
    assert_execution();
    return _output_buffer;
}

PSha256_t
OntBlockCompressionTask::prev_block_hash() {
    assert_execution();
    return _prev_block_hash;
}

PSha256_t
OntBlockCompressionTask::block_hash() {
    assert_execution();
    return _block_hash;
}

PSha256_t
OntBlockCompressionTask::compressed_block_hash() {
    assert_execution();
    return _compressed_block_hash;
}

size_t OntBlockCompressionTask::txn_count() {
    return _txn_count;
}

const std::vector<unsigned int>& OntBlockCompressionTask::short_ids() {
    assert_execution();
    return _short_ids;
}

const std::vector<unsigned int>& OntBlockCompressionTask::ignored_short_ids() {
    assert_execution();
    return _ignored_short_ids;
}

size_t OntBlockCompressionTask::get_task_byte_size() const {
    size_t sub_tasks_size = 0;
    for (const TaskData& sub_task_data: _sub_tasks) {
        const PSubTask_t& p_task = sub_task_data.sub_task;
        sub_tasks_size += (p_task->output_buffer().capacity() + sizeof(p_task) + sizeof(OntBlockCompressionSubTask));
    }
    size_t block_buffer_size = 0;
    if (_block_buffer != nullptr) {
        block_buffer_size = _block_buffer->size();
    }
    return sizeof(OntBlockCompressionTask) + _output_buffer->capacity() + block_buffer_size +
           (_short_ids.capacity() * sizeof(uint32_t)) + sub_tasks_size;
}

void OntBlockCompressionTask::cleanup() {
    assert_execution();
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void OntBlockCompressionTask::_execute(SubPool_t& sub_pool) {
    utils::protocols::ontology::OntBlockMessage msg(*_block_buffer);
    _prev_block_hash = std::make_shared<Sha256_t>(msg.prev_block_hash());
    _block_hash = std::make_shared<Sha256_t>(msg.block_hash());
    uint32_t tx_count = 0;
    size_t offset = msg.get_txn_count(tx_count);
    _txn_count = tx_count;
    size_t last_idx = _dispatch(tx_count, msg, offset, sub_pool);
    size_t output_offset = sizeof(uint64_t);
    constexpr uint8_t is_consensus_flag = 0x00;
    output_offset = utils::common::set_little_endian_value(*_output_buffer, is_consensus_flag, output_offset);
    auto merkle_root = msg.merkle_root();
    output_offset = _output_buffer->copy_from_buffer(merkle_root, output_offset, 0, merkle_root.size());
    output_offset = _output_buffer->copy_from_buffer(
        *_block_buffer,
        output_offset,
        0,
        offset
    );
    for (size_t idx = 0 ; idx <= last_idx ; ++idx) {
        TaskData& data = _sub_tasks.at(idx);
        data.sub_task->wait();
        output_offset = _on_sub_task_completed(*data.sub_task);
    }
    _set_output_buffer(output_offset);
    _compressed_block_hash = std::make_shared<Sha256_t>(
        utils::crypto::double_sha256(
                _output_buffer->array(),
                0,
                _output_buffer->size()
        )
    );
}

void OntBlockCompressionTask::_init_sub_tasks(
    size_t pool_size,
    size_t tx_count
)
{
    if (_sub_tasks.size() < pool_size) {
        size_t default_count = ONT_DEFAULT_TX_COUNT;
        size_t capacity = std::max(
            default_count,
            (size_t) (tx_count / pool_size)
        ) * ONT_DEFAULT_TX_SIZE;
        for (size_t i = _sub_tasks.size() ; i < pool_size ; ++i)
        {
            TaskData task_data;
            task_data.sub_task = std::make_shared<
                OntBlockCompressionSubTask>(
                capacity
            );
            task_data.offsets = std::make_shared<TXOffsets_t>();
            _sub_tasks.push_back(std::move(task_data));
        }
    } else {
        for (auto& task_data : _sub_tasks) {
            task_data.offsets->clear();
        }
    }
}

size_t OntBlockCompressionTask::_dispatch(
    uint32_t tx_count,
    utils::protocols::ontology::OntBlockMessage& msg,
    size_t offset,
    SubPool_t& sub_pool
)
{
    size_t pool_size = sub_pool.size(), prev_idx = 0;
    _init_sub_tasks(pool_size, tx_count);
    size_t bulk_size = std::max(
        (size_t) (tx_count / pool_size),
        std::max(pool_size, _minimal_tx_count)
    );
    size_t idx = 0;
    for (uint32_t count = 0 ; count < tx_count ; ++count) {
        size_t from = offset;
        idx = std::min((size_t) (count / bulk_size), pool_size - 1);
        offset = msg.get_next_tx_offset(offset);
        _sub_tasks[idx].offsets->push_back(
            std::make_tuple(from, offset, 0)
        );
        if(idx > prev_idx) {
            _enqueue_task(prev_idx, sub_pool);
        }
        prev_idx = idx;
    }
    _enqueue_task(prev_idx, sub_pool);
    return prev_idx;
}

size_t OntBlockCompressionTask::_on_sub_task_completed(
    OntBlockCompressionSubTask& tsk
)
{
    auto& output_buffer = tsk.output_buffer();
    auto& short_ids = tsk.short_ids();
    auto& ignored_short_ids = tsk.ignored_short_ids();

    _output_buffer->operator +=(output_buffer);
    _short_ids.reserve(_short_ids.size() + short_ids.size());
    _short_ids.insert(_short_ids.end(), short_ids.begin(), short_ids.end());
    _ignored_short_ids.reserve(_ignored_short_ids.size() + ignored_short_ids.size());
    _ignored_short_ids.insert(_ignored_short_ids.end(), ignored_short_ids.begin(), ignored_short_ids.end());
    return _output_buffer->size();
}

void OntBlockCompressionTask::_set_output_buffer(
    size_t output_offset
)
{
    utils::common::set_little_endian_value(
        *_output_buffer,
        (uint64_t)output_offset,
        0
    );
    const unsigned int short_ids_size = _short_ids.size();
    output_offset = utils::common::set_little_endian_value(
        *_output_buffer,
        short_ids_size,
        output_offset
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

void OntBlockCompressionTask::_enqueue_task(
    size_t task_idx,
    SubPool_t& sub_pool
)
{
    TaskData& data = _sub_tasks[task_idx];
    data.sub_task->init(
        _tx_service,
        _block_buffer.get(),
        data.offsets,
        _enable_block_compression,
        _min_tx_age_seconds
    );
    sub_pool.enqueue_task(data.sub_task);
}

} // task
