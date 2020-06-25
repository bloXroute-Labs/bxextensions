#include <iostream>
#include <cstdint>
#include <list>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/task/eth_task_types.h"


#ifndef TPE_TASK_SUB_TASK_ETH_BLOCK_DECOMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_ETH_BLOCK_DECOMPRESSION_SUB_TASK_H_

namespace task {

typedef std::vector<unsigned int> ShortIDs_t;

class EthBlockDecompressionSubTask : public SubTaskBase {

struct TaskData {
    TaskData(): short_ids_offset(0), short_ids_len(0), output_offset(0) {
        offsets = std::make_shared<EthTXOffsets_t>();
    }

    void clear() {
        short_ids_offset = 0;
        short_ids_len = 0;
        output_offset = 0;
        offsets->clear();
    }

    PEthOffests_t offsets;
    size_t short_ids_offset;
    size_t short_ids_len;
    size_t output_offset;
};

public:
    EthBlockDecompressionSubTask();

    void init(
        PTransactionService_t tx_service,
        const BlockBuffer_t* block_buffer,
        utils::common::ByteArray* output_buffer,
        PEthOffests_t tx_offsets,
        const ShortIDs_t* short_ids
    );

    const ShortIDs_t* short_ids();
    const UnknownTxHashes_t& unknown_tx_hashes();
    const UnknownTxSIDs_t& unknown_tx_sids();
    bool success();
    TaskData& task_data();

protected:
    void _execute() override;

private:

    utils::common::ByteArray* _output_buffer;
    const BlockBuffer_t* _block_buffer;
    PEthOffests_t _tx_offsets;
    PTransactionService_t _tx_service;
    const ShortIDs_t* _short_ids;
    size_t _content_size;
    UnknownTxHashes_t _unknown_tx_hashes;
    UnknownTxSIDs_t _unknown_tx_sids;
    bool _success;
    TaskData _task_data;
};

} // task



#endif /* TPE_TASK_SUB_TASK_ETH_BLOCK_DECOMPRESSION_SUB_TASK_H_ */
