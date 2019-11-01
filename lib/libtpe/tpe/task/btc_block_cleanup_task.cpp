#include "tpe/task/btc_block_cleanup_task.h"
#include "tpe/task/sub_task/btc_block_cleanup_sub_task.h"

#include <utils/crypto/hash_helper.h>

namespace task {

BtcBlockCleanupTask::BtcBlockCleanupTask(
        size_t minimal_tx_count/* = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/
):
    MainTaskBase(),
    _tx_service(nullptr),
    _minimal_tx_count(minimal_tx_count),
    _tx_count(0)
{
    _block_buffer = std::make_shared<BlockBuffer_t>(BlockBuffer_t::empty());
}


void BtcBlockCleanupTask::init(
        PBlockBuffer_t block_buffer,
        PTransactionService_t tx_service
)
{
    _tx_service = std::move(tx_service);
    _block_buffer = std::move(block_buffer);
    _short_ids.clear();
    _all_tx_hashes.clear();
    _unknown_tx_hashes.clear();
    _tx_count = 0;
}

const std::vector<unsigned int>& BtcBlockCleanupTask::short_ids() {
    assert_execution();
    return _short_ids;
}

const std::vector<Sha256_t>& BtcBlockCleanupTask::unknown_tx_hashes() {
    assert_execution();
    return _unknown_tx_hashes;
}

uint64_t BtcBlockCleanupTask::tx_count() {
    assert_execution();
    return _tx_count;
}

size_t BtcBlockCleanupTask::total_content_removed() {
   assert_execution();
    return _total_content_removed;
}

size_t BtcBlockCleanupTask::get_task_byte_size() const {
    size_t block_buffer_size = 0;
    if (_block_buffer != nullptr) {
        block_buffer_size = _block_buffer->size();
    }
    return sizeof(BtcBlockCleanupTask) +
        (_all_tx_hashes.capacity() + _unknown_tx_hashes.capacity()) * sizeof(Sha256_t) +
        _short_ids.capacity() * sizeof(uint32_t) + block_buffer_size;
}

void BtcBlockCleanupTask::cleanup() {
    assert_execution();
    _clean_transactions();
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void BtcBlockCleanupTask::_execute(SubPool_t &sub_pool) {
    utils::protocols::bitcoin::BtcBlockMessage msg(*_block_buffer);
    size_t offset = msg.get_tx_count(_tx_count);
    size_t from;
    _short_ids.reserve(_tx_count * MAX_DUPLICATES_SIDS);
    _all_tx_hashes.reserve(_tx_count);
    _unknown_tx_hashes.reserve(_tx_count);
    const size_t pool_size = sub_pool.size();
    size_t bulk_size = std::max(
            (size_t) (_tx_count / pool_size),
            std::max(pool_size, _minimal_tx_count)
    );
    POffests_t offsets = std::make_shared<TXOffsets_t>();
    size_t idx = 0, prev_idx = 0;
    std::list<std::shared_ptr<BtcBlockCleanupSubTask>> sub_tasks;
    for (int count = 0 ; count < _tx_count ; ++count) {
        from = offset;
        idx = std::min((size_t) (count / bulk_size), pool_size - 1);
        size_t witness_offset;
        offset = msg.get_next_tx_offset(offset, witness_offset);
        offsets->push_back(std::make_tuple(from, witness_offset, offset));
        if(idx > prev_idx) {
            auto sub_task = std::make_shared<BtcBlockCleanupSubTask>(
                  _tx_service,
                  *_block_buffer,
                  std::move(offsets),
                  _short_ids,
                  _all_tx_hashes,
                  _unknown_tx_hashes
            );
            offsets = std::make_shared<TXOffsets_t>();
            sub_pool.enqueue_task(sub_task);
            sub_tasks.push_back(std::move(sub_task));
        }
        prev_idx = idx;
    }
    auto sub_task = std::make_shared<BtcBlockCleanupSubTask>(
            _tx_service,
            *_block_buffer,
            std::move(offsets),
            _short_ids,
            _all_tx_hashes,
            _unknown_tx_hashes
    );
    offsets = nullptr;
    sub_pool.enqueue_task(sub_task);
    sub_tasks.push_back(std::move(sub_task));
    for (auto& pending_sub_task: sub_tasks) {
        pending_sub_task->wait();
    }
}

void BtcBlockCleanupTask::_clean_transactions() {
    _total_content_removed = _tx_service->remove_transactions_by_hashes(_all_tx_hashes);
}

} // task
