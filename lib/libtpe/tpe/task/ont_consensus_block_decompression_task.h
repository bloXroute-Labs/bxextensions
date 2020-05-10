#include <iostream>
#include <memory>
#include <list>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/ontology/ont_consts.h>
#include <utils/protocols/ontology/consensus/bx_ont_consensus_message.h>

#include "tpe/task/btc_task_types.h"
#include "tpe/task/main_task_base.h"
#include "tpe/consts.h"

#ifndef TPE_TASK_ONT_CONSENSUS_BLOCK_DECOMPRESSION_TASK_H_
#define TPE_TASK_ONT_CONSENSUS_BLOCK_DECOMPRESSION_TASK_H_

namespace task {

typedef utils::protocols::ontology::consensus::BxOntConsensusMessage BxOntConsensusMessage_t;

class OntConsensusBlockDecompressionTask : public MainTaskBase {

public:
    explicit OntConsensusBlockDecompressionTask(
            size_t capacity = ONT_DEFAULT_BLOCK_SIZE,
            size_t minimal_tx_count = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
    );

    void init(
            PBlockBuffer_t block_buffer,
            PTransactionService_t tx_service
    );

    PByteArray_t block_message();
    const UnknownTxHashes_t& unknown_tx_hashes();
    const UnknownTxSIDs_t& unknown_tx_sids();
    PSha256_t block_hash();
    bool success();
    uint32_t txn_count();
    const std::vector<unsigned int>& short_ids();

    size_t get_task_byte_size() const override;

    void cleanup() override;

protected:
    void _execute(SubPool_t& sub_pool) override;

private:

    PBlockBuffer_t _block_buffer;
    PByteArray_t _output_buffer;
    UnknownTxHashes_t _unknown_tx_hashes;
    UnknownTxSIDs_t _unknown_tx_sids;
    std::vector<unsigned int> _short_ids;
    PSha256_t _block_hash;
    PTransactionService_t _tx_service;
    const size_t _minimal_tx_count;
    bool _success;
    uint32_t _txn_count;
};

} // task

#endif //TPE_TASK_ONT_CONSENSUS_BLOCK_DECOMPRESSION_TASK_H_
