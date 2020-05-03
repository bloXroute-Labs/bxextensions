#include <iostream>
#include <cstdint>
#include <list>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/crypto/hash_helper.h>
#include <utils/protocols/bitcoin/btc_consts.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/task/btc_task_types.h"

#ifndef TPE_TASK_ONT_BLOCK_CLEANUP_SUB_TASK_H_
#define TPE_TASK_ONT_BLOCK_CLEANUP_SUB_TASK_H_

namespace task {

class OntBlockCleanupSubTask : public SubTaskBase {

public:

    OntBlockCleanupSubTask(
            PTransactionService_t tx_service,
            const BlockBuffer_t& block_buffer,
            POffests_t tx_offsets,
            std::vector<unsigned int>& short_ids,
            std::vector<Sha256_t>& all_tx_hashes,
            std::vector<Sha256_t>& unknown_tx_hashes
    );



protected:
    void _execute() override;

private:

    const BlockBuffer_t& _block_buffer;
    POffests_t _tx_offsets;
    PTransactionService_t _tx_service;
    std::vector<unsigned int>& _short_ids;
    std::vector<Sha256_t>& _all_tx_hashes;
    std::vector<Sha256_t>& _unknown_tx_hashes;
};

} // task

#endif //TPE_TASK_ONT_BLOCK_CLEANUP_SUB_TASK_H_
