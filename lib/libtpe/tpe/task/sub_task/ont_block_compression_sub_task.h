#include <iostream>
#include <cstdint>
#include <list>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/ontology/ont_consts.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/task/btc_task_types.h"


#ifndef TPE_TASK_SUB_TASK_ONT_BLOCK_COMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_ONT_BLOCK_COMPRESSION_SUB_TASK_H_

namespace task {

class OntBlockCompressionSubTask : public SubTaskBase {

public:
    OntBlockCompressionSubTask(size_t capacity);

    void init(
        PTransactionService_t tx_service,
        const BlockBuffer_t* block_buffer,
        POffests_t tx_offsets,
        bool enable_block_compression
    );

    const utils::common::ByteArray& output_buffer();
    const std::vector<unsigned int>& short_ids();


protected:
    void _execute() override;

private:

    utils::common::ByteArray _output_buffer;
    const BlockBuffer_t* _block_buffer;
    POffests_t _tx_offsets;
    PTransactionService_t _tx_service;
    std::vector<unsigned int> _short_ids;
    bool _enable_block_compression;
};

}

#endif //TPE_TASK_SUB_TASK_ONT_BLOCK_COMPRESSION_SUB_TASK_H_
