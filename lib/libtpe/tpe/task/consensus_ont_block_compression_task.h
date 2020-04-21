#include <iostream>
#include <memory>
#include <list>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/ontology/ont_consts.h>
#include <utils/protocols/ontology/consensus/consensus_ont_message.h>

#include "tpe/task/btc_task_types.h"
#include "tpe/task/main_task_base.h"
#include "tpe/consts.h"

#ifndef TPE_TASK_CONSENSUS_ONT_BLOCK_COMPRESSION_TASK_H_
#define TPE_TASK_CONSENSUS_ONT_BLOCK_COMPRESSION_TASK_H_

namespace task {

typedef utils::protocols::ontology::consensus::ConsensusOntMessage ConsensusOntMessage_t;

class ConsensusOntBlockCompressionTask : public MainTaskBase {

    struct TaskData {
        TaskData(): offsets(nullptr) {}
        TaskData(TaskData&& other) noexcept {
            offsets = std::move(other.offsets);
        }

        std::shared_ptr<TXOffsets_t> offsets;
    };

    typedef std::vector<TaskData> SubTasksData_t;

public:
    explicit ConsensusOntBlockCompressionTask(
            size_t capacity = ONT_DEFAULT_BLOCK_SIZE,
            size_t minimal_tx_count = ONT_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
    );

    void init(
            PBlockBuffer_t block_buffer,
            PTransactionService_t tx_service
    );

    PByteArray_t bx_block();

    PSha256_t prev_block_hash();
    PSha256_t block_hash();
    PSha256_t compressed_block_hash();

    size_t txn_count();

    const std::vector<unsigned int>& short_ids();

    size_t get_task_byte_size() const override;

    void cleanup() override;

protected:
    void _execute(SubPool_t& sub_pool) override;

private:

    PBlockBuffer_t _block_buffer;
    PByteArray_t _output_buffer;
    PTransactionService_t _tx_service;
    SubTasksData_t _sub_tasks;
    const size_t _minimal_tx_count;
    PSha256_t _prev_block_hash, _block_hash;
    PSha256_t _compressed_block_hash;
    uint32_t _txn_count;
    std::vector<unsigned int> _short_ids;
};

} // task

#endif //TPE_TASK_CONSENSUS_ONT_BLOCK_COMPRESSION_TASK_H_

