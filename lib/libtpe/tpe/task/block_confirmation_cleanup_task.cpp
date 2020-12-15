#include "tpe/task/block_confirmation_cleanup_task.h"
#include <utils/protocols/internal/block_confirmation_message.h>

namespace task {

typedef utils::protocols::internal::BlockConfirmationMessage BlockConfirmationMessage_t;

BlockConfirmationCleanupTask::BlockConfirmationCleanupTask():
        _tx_service(nullptr),
        _total_content_removed(0),
        _tx_count(0)
{
    _msg_buffer = std::make_shared<BufferView_t>(BufferView_t::empty());
}

void BlockConfirmationCleanupTask::init(PBufferView_t msg_buffer, PTransactionService_t tx_service) {
    _msg_buffer = std::move(msg_buffer);
    _tx_service = std::move(tx_service);
    _block_hash.clear();
    _total_content_removed = 0;
    _tx_count = 0;
    _short_ids.clear();
}

const Sha256_t& BlockConfirmationCleanupTask::block_hash() {
    assert_execution();
    return _block_hash;
}

uint32_t BlockConfirmationCleanupTask::tx_count() {
    assert_execution();
    return _tx_count;
}

size_t BlockConfirmationCleanupTask::total_content_removed() {
    assert_execution();
    return _total_content_removed;
}

const std::vector<uint32_t>& BlockConfirmationCleanupTask::short_ids() {
    assert_execution();
    return _short_ids;
}

size_t BlockConfirmationCleanupTask::get_task_byte_size() const {
    size_t msg_buffer_size = 0;
    if (_msg_buffer != nullptr) {
        msg_buffer_size = _msg_buffer->size();
    }
    return sizeof(BlockConfirmationCleanupTask) + _short_ids.capacity() * sizeof(uint32_t) +
            msg_buffer_size;
}

void BlockConfirmationCleanupTask::cleanup() {
    assert_execution();
    BlockConfirmationMessage_t msg(*_msg_buffer);
    size_t offset = msg.parse_block_hash(_block_hash);
    uint32_t short_ids_count, tx_hash_count, short_id;
    Sha256_t sha;
    offset = msg.parse_short_ids_count(offset, short_ids_count);
    _short_ids.reserve(short_ids_count);
    for (uint32_t idx = 0 ; idx < short_ids_count ; ++idx) {
        offset = msg.parse_next_short_id(offset, short_id);
        _short_ids.push_back(short_id);
        _total_content_removed += _tx_service->remove_transaction_by_short_id(short_id, _short_ids);
    }
    offset = msg.parse_tx_hash_count(offset, tx_hash_count);
    for (uint32_t idx = 0 ; idx < tx_hash_count ; ++idx) {
        offset = msg.parse_next_tx_hash(offset, sha);
        auto& tx_hash_to_short_ids = _tx_service->get_tx_hash_to_short_ids();
        auto short_ids_iter = tx_hash_to_short_ids.find(sha);
        if (short_ids_iter != tx_hash_to_short_ids.end()) {
            auto& short_ids = short_ids_iter->second;
            for (uint32_t short_id: short_ids) {
                _short_ids.push_back(short_id);
            }
        }
        _total_content_removed += _tx_service->remove_transaction_by_hash(sha);
    }
    _tx_count = tx_hash_count + short_ids_count;
    _tx_service->on_block_cleaned_up(_block_hash);
    _msg_buffer = nullptr;
    _tx_service = nullptr;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void BlockConfirmationCleanupTask::_execute(SubPool_t &sub_pool) {
}
#pragma GCC diagnostic pop

} // task