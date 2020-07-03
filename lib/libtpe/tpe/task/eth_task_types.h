#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>

#include <utils/crypto/sha256.h>
#include <utils/common/buffer_view.h>
#include <utils/common/byte_array.h>

#include "tpe/service/transaction_service.h"

#ifndef TPE_TASK_ETH_TASK_TYPES_H_
#define TPE_TASK_ETH_TASK_TYPES_H_

namespace task {

    typedef service::TransactionService TransactionService_t;
    typedef std::shared_ptr<TransactionService_t> PTransactionService_t;
    typedef utils::common::BufferView BlockBuffer_t;
    typedef std::shared_ptr<BlockBuffer_t> PBlockBuffer_t;
    typedef utils::common::BufferView TxBuffer_t;
    typedef service::ShortIDToSha256Map_t ShortIDToSha256Map_t;
    typedef std::list<std::tuple<size_t, size_t, uint64_t>> EthTXOffsets_t;
    typedef std::shared_ptr<EthTXOffsets_t> PEthOffests_t;
    typedef service::UnknownTxHashes_t UnknownTxHashes_t;
    typedef service::ShortIDs_t UnknownTxSIDs_t;
    typedef service::Sha256_t Sha256_t;
    typedef service::PSha256_t PSha256_t;
    typedef utils::common::ByteArray ByteArray_t;
    typedef std::shared_ptr<ByteArray_t> PByteArray_t;


} // task

#endif /* TPE_TASK_ETH_TASK_TYPES_H_ */
