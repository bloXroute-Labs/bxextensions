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


//TEST_F(CompactBlockCompression, test_block_00000000000002466e6ff00b90b49f1588b44f058624e75a55923e7ad967795c)
//{
//    return;
//}

