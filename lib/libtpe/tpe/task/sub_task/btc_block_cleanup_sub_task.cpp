#include "tpe/task/sub_task/btc_block_cleanup_sub_task.h"

namespace task {

BtcBlockCleanupSubTask::BtcBlockCleanupSubTask(
        PTransactionService_t tx_service,
        const BlockBuffer_t& block_buffer,
        POffests_t tx_offsets,
        std::vector<unsigned int>& short_ids,
        std::vector<Sha256_t>& all_tx_hashes,
        std::vector<Sha256_t>& unknown_tx_hashes
) :
    SubTaskBase(),
    _block_buffer(block_buffer),
    _short_ids(short_ids),
    _all_tx_hashes(all_tx_hashes),
    _unknown_tx_hashes(unknown_tx_hashes)
{
    _tx_service = std::move(tx_service);
    _tx_offsets = std::move(tx_offsets);
}

void BtcBlockCleanupSubTask::_execute() {
    for (const auto& offsets: *_tx_offsets) {
        size_t from = offsets.first;
        size_t to = offsets.second;
        const Sha256_t sha(std::move(utils::crypto::double_sha256(
                _block_buffer, from, to - from
        )));
        if (_tx_service->has_short_id(sha)) {
            for (unsigned int short_id: _tx_service->tx_hash_to_short_ids()[sha]) {
                _short_ids.push_back(short_id);
            }
        } else {
            _unknown_tx_hashes.push_back(sha);
        }
        _all_tx_hashes.push_back(sha);
    }
}

} // task
