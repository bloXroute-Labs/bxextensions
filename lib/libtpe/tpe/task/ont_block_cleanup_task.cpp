#include "tpe/task/ont_block_cleanup_task.h"
#include "tpe/task/sub_task/ont_block_cleanup_sub_task.h"

#include <utils/crypto/hash_helper.h>

namespace task {

OntBlockCleanupTask::OntBlockCleanupTask(
        size_t minimal_tx_count/* = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/
):
        MainTaskBase(),
        _tx_service(nullptr),
        _minimal_tx_count(minimal_tx_count),
        _txn_count(0),
        _total_content_removed(0)
{
    _block_buffer = std::make_shared<BlockBuffer_t>(BlockBuffer_t::empty());
}


void OntBlockCleanupTask::init(
        PBlockBuffer_t block_buffer,
        PTransactionService_t tx_service
)
{
    _tx_service = std::move(tx_service);
    _block_buffer = std::move(block_buffer);
    _short_ids.clear();
    _all_tx_hashes.clear();
    _unknown_tx_hashes.clear();
    _txn_count = 0;
    _total_content_removed = 0;
}

const std::vector<unsigned int>& OntBlockCleanupTask::short_ids() {
    assert_execution();
    return _short_ids;
}

const std::vector<Sha256_t>& OntBlockCleanupTask::unknown_tx_hashes() {
    assert_execution();
    return _unknown_tx_hashes;
}

uint32_t OntBlockCleanupTask::txn_count() {
    assert_execution();
    return _txn_count;
}

size_t OntBlockCleanupTask::total_content_removed() {
    assert_execution();
    return _total_content_removed;
}

size_t OntBlockCleanupTask::get_task_byte_size() const {
    size_t block_buffer_size = 0;
    if (_block_buffer != nullptr) {
        block_buffer_size = _block_buffer->size();
    }
    return sizeof(OntBlockCleanupTask) +
           (_all_tx_hashes.capacity() + _unknown_tx_hashes.capacity()) * sizeof(Sha256_t) +
           _short_ids.capacity() * sizeof(uint32_t) + block_buffer_size;
}

void OntBlockCleanupTask::cleanup() {
    assert_execution();
    _clean_transactions();
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void OntBlockCleanupTask::_execute(SubPool_t &sub_pool) {
    utils::protocols::ontology::OntBlockMessage msg(*_block_buffer);
    size_t offset = msg.get_txn_count(_txn_count);
    size_t from;
    _short_ids.reserve(_txn_count * MAX_DUPLICATES_SIDS);
    _all_tx_hashes.reserve(_txn_count);
    _unknown_tx_hashes.reserve(_txn_count);
    const size_t pool_size = sub_pool.size();
    size_t bulk_size = std::max(
            (size_t) (_txn_count / pool_size),
            std::max(pool_size, _minimal_tx_count)
    );
    POffests_t offsets = std::make_shared<TXOffsets_t>();
    size_t idx = 0, prev_idx = 0;
    std::list<std::shared_ptr<OntBlockCleanupSubTask>> sub_tasks;
    for (uint32_t count = 0 ; count < _txn_count ; ++count) {
        from = offset;
        idx = std::min((size_t) (count / bulk_size), pool_size - 1);
        offset = msg.get_next_tx_offset(offset);
        offsets->push_back(std::make_tuple(from, 0, offset));
        if(idx > prev_idx) {
            auto sub_task = std::make_shared<OntBlockCleanupSubTask>(
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
    auto sub_task = std::make_shared<OntBlockCleanupSubTask>(
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

void OntBlockCleanupTask::_clean_transactions() {
    _total_content_removed = _tx_service->remove_transactions_by_hashes(_all_tx_hashes);
}

} // task
