#include <iostream>
#include <memory>
#include <list>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/ethereum/eth_consts.h>
#include <utils/protocols/ethereum/eth_block_message.h>

#include "tpe/task/eth_task_types.h"
#include "tpe/task/main_task_base.h"
#include "tpe/consts.h"


#ifndef TPE_TASK_ETH_BLOCK_COMPRESSION_TASK_H
#define TPE_TASK_ETH_BLOCK_COMPRESSION_TASK_H

namespace task {

class EthBlockCompressionTask : public MainTaskBase {

public:
    explicit EthBlockCompressionTask(
        size_t capacity = ETH_DEFAULT_BLOCK_SIZE,
        size_t minimal_tx_count = ETH_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
    );

    void init(
        PBlockBuffer_t block_buffer,
        PTransactionService_t tx_service,
        bool enable_block_compression,
        float min_tx_age_seconds
    );

    PByteArray_t bx_block();
    PSha256_t prev_block_hash();
    PSha256_t block_hash();
    PSha256_t compressed_block_hash();
    size_t txn_count();
    size_t starting_offset();
    const std::vector<unsigned int>& short_ids();

    size_t get_task_byte_size() const override;

    void cleanup() override;

protected:
    void _execute(SubPool_t& sub_pool) override;

private:
    PBlockBuffer_t _block_buffer;
    BlockBuffer_t _block_header, _block_trailer;
    PByteArray_t _output_buffer;
    PTransactionService_t _tx_service;
    const size_t _minimal_tx_count;
    PSha256_t _prev_block_hash, _block_hash, _compressed_block_hash;
    size_t _txn_count;
    std::vector<unsigned int> _short_ids;
    size_t _content_size;
    size_t _starting_offset;
    bool _enable_block_compression;
    float _min_tx_age_seconds;
};

} // task


#endif /* TPE_TASK_ETH_BLOCK_COMPRESSION_TASK_H */
