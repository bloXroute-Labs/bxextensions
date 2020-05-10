#include <iostream>

#include <utils/protocols/ontology/ont_block_message.h>
#include <utils/protocols/ontology/ont_consts.h>

#include "tpe/task/main_task_base.h"
#include "tpe/task/btc_task_types.h"

#ifndef TPE_TASK_ONT_BLOCK_CLEANUP_TASK_H_
#define TPE_TASK_ONT_BLOCK_CLEANUP_TASK_H_

#define MAX_DUPLICATES_SIDS 4

namespace task {

class OntBlockCleanupTask : public MainTaskBase {

public:
    explicit OntBlockCleanupTask(size_t minimal_tx_count = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT);

    void init(
            PBlockBuffer_t block_buffer,
            PTransactionService_t tx_service
    );

    const std::vector<unsigned int>& short_ids();
    const std::vector<Sha256_t>& unknown_tx_hashes();
    uint32_t txn_count();
    size_t total_content_removed();
    size_t get_task_byte_size() const override;

    void cleanup() override;

protected:
    void _execute(SubPool_t& sub_pool) override;

private:
    void _clean_transactions();

    PBlockBuffer_t _block_buffer;
    PTransactionService_t _tx_service;
    std::vector<unsigned int> _short_ids;
    std::vector<Sha256_t> _all_tx_hashes;
    std::vector<Sha256_t> _unknown_tx_hashes;
    size_t _minimal_tx_count;
    uint32_t _txn_count;
    size_t _total_content_removed;
};

} // task

#endif //TPE_TASK_ONT_BLOCK_CLEANUP_TASK_H_
