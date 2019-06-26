#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <string>

#include "utils/common/string_helper.h"
#include "utils/common/buffer_view.h"
#include "utils/crypto/hash_helper.h"

#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/protocols/bitcoin/btc_block_message.h"
#include "utils/protocols/bitcoin/btc_compact_block_message.h"

#include "tpe/service/transaction_service.h"

#include "tpe/task/btc_task_types.h"
#include "tpe/task/btc_block_compression_task.h"
#include "tpe/task_pool_executor.h"

typedef utils::common::BufferView BufferView_t;
typedef std::shared_ptr<BufferView_t> PBufferView_t;

typedef utils::crypto::Sha256 Sha256_t;

typedef service::TransactionService TransactionService_t;
typedef std::shared_ptr<TransactionService_t> PTransactionService_t;


class CompactBlockCompression : public ::testing::Test {
};


TEST_F(CompactBlockCompression, test_block_00000000000002466e6ff00b90b49f1588b44f058624e75a55923e7ad967795c)
{
    return;
    const unsigned int magic = 4206867930;

    // read blocks from filesystem
    std::string full_block_message_hex_string;
    std::ifstream full_block_file("../../../lib/libtpe/test/data/normalblock-00000000000002466e6ff00b90b49f1588b44f058624e75a55923e7ad967795c");
    if (full_block_file.is_open())
    {
        getline(full_block_file, full_block_message_hex_string);
        full_block_file.close();
    }
    std::vector<uint8_t> full_block_vec;
    utils::common::from_hex_string(full_block_message_hex_string, full_block_vec);
    BufferView_t full_block_message(full_block_vec);
    std::string compact_block_message_hex_string;

    std::ifstream cmpt_block_file("../../../lib/libtpe/test/data/compactblock-00000000000002466e6ff00b90b49f1588b44f058624e75a55923e7ad967795c");
    if (cmpt_block_file.is_open())
    {
        getline(cmpt_block_file, compact_block_message_hex_string);
        cmpt_block_file.close();
    }

    // construct the transaction_service from the full block
    PTransactionService_t tx_service = std::move(std::make_shared<TransactionService_t>(1));
    uint32_t short_id = 1;

    // loop over full block and insert the tx
    utils::protocols::bitcoin::BtcBlockMessage btc_block_message(full_block_message);
    uint64_t txs_count;
    size_t offset = btc_block_message.get_tx_count(txs_count);
    size_t off;
    Sha256_t sha;

    for (size_t i = 0; i < txs_count; ++i)
    {
        off = offset;

        offset = btc_block_message.get_next_tx_offset(offset);
        sha = std::move(utils::crypto::double_sha256(full_block_message, off, offset - off));
        tx_service->get_tx_hash_to_short_ids()[sha].insert(short_id);
        ++short_id;
    }

    // compress the block
//    task::pool::TaskPoolExecutor task_pool_executor();
//    task::BtcBlockCompressionTask btc_block_compression_task;
//    btc_block_compression_task.init(full_block_message, tx_service);

    // btc_block_compression_task._execute enqueue_task
    // task_pool_executor.enqueue_task(std::make_shared<task::BtcBlockCompressionTask>(btc_block_compression_task));
    // read compact block
    // compact block mapping
    // compact block compress
    // compare 2 blocks
}

