#include <iostream>

#include <utils/protocols/bitcoin/btc_block_message.h>

#include "tpe/task/main_task_base.h"
#include "tpe/task/btc_task_types.h"
#include <utils/protocols/bitcoin/btc_consts.h>

#ifndef TPE_TASK_BTC_BLOCK_CLEANUP_TASK_H_
#define TPE_TASK_BTC_BLOCK_CLEANUP_TASK_H_

namespace task {

class BtcBlockCleanupTask : public MainTaskBase {

public:
    explicit BtcBlockCleanupTask(size_t minimal_tx_count = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT);

    void init(
            BlockBuffer_t block_buffer,
            PTransactionService_t tx_service
    );

    const std::vector<unsigned int>& short_ids();
    const std::vector<Sha256_t>& unknown_tx_hashes();
    uint64_t tx_count();
    size_t total_content_removed();
    size_t get_task_byte_size() const override;

protected:
    void _execute(SubPool_t& sub_pool) override;

private:
    void _clean_transactions();

    BlockBuffer_t _block_buffer;
    PTransactionService_t _tx_service;
    std::vector<unsigned int> _short_ids;
    std::vector<Sha256_t> _all_tx_hashes;
    std::vector<Sha256_t> _unknown_tx_hashes;
    size_t _minimal_tx_count;
    uint64_t _tx_count;
    size_t _total_content_removed;
};

} // task

#endif //TPE_TASK_BTC_BLOCK_CLEANUP_TASK_H_
