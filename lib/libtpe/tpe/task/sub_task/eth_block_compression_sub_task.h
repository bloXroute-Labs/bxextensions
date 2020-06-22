#include <iostream>
#include <cstdint>
#include <list>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/task/eth_task_types.h"


#ifndef TPE_TASK_SUB_TASK_ETH_BLOCK_COMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_ETH_BLOCK_COMPRESSION_SUB_TASK_H_

namespace task {

class EthBlockCompressionSubTask : public SubTaskBase {

public:
    EthBlockCompressionSubTask(size_t capacity);

    void init(
            PTransactionService_t tx_service,
            const BlockBuffer_t* block_buffer,
            PEthOffests_t tx_offsets
    );

    const utils::common::ByteArray& output_buffer();
    const std::vector<unsigned int>& short_ids();
    size_t content_size() const;


protected:
    void _execute() override;

private:

    utils::common::ByteArray _output_buffer;
    const BlockBuffer_t* _block_buffer;
    PEthOffests_t _tx_offsets;
    PTransactionService_t _tx_service;
    std::vector<unsigned int> _short_ids;
    size_t _content_size;
};

} // task



#endif /* TPE_TASK_SUB_TASK_ETH_BLOCK_COMPRESSION_SUB_TASK_H_ */
