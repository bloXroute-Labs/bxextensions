#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>

#include <utils/crypto/sha256.h>
#include <utils/common/buffer_view.h>
#include <utils/common/byte_array.h>

#include "tpe/service/transaction_service.h"

#ifndef TPE_TASK_BTC_TASK_TYPES_H_
#define TPE_TASK_BTC_TASK_TYPES_H_

namespace task {

typedef service::TransactionService TransactionService_t;
typedef std::shared_ptr<TransactionService_t> PTransactionService_t;
typedef utils::common::BufferView BlockBuffer_t;
typedef utils::common::BufferView TxBuffer_t;
typedef service::Sha256ToShortIDsMap_t Sha256ToShortIDsMap_t;
typedef service::ShortIDToSha256Map_t ShortIDToSha256Map_t;
typedef service::Sha256ToContentMap_t Sha256ToContentMap_t;
typedef std::list<std::pair<size_t, size_t>> TXOffsets_t;
typedef std::shared_ptr<TXOffsets_t> POffests_t;
typedef service::UnknownTxHashes_t UnknownTxHashes_t;
typedef service::UnknownTxSIDs_t UnknownTxSIDs_t;
typedef service::Sha256_t Sha256_t;
typedef service::PSha256_t PSha256_t;
typedef utils::common::ByteArray ByteArray_t;
typedef std::shared_ptr<ByteArray_t> PByteArray_t;


} // task

#endif /* TPE_TASK_BTC_TASK_TYPES_H_ */
