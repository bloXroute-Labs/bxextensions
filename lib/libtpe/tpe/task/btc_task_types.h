#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>

#include <utils/crypto/sha256.h>

#ifndef TPE_TASK_BTC_TASK_TYPES_H_
#define TPE_TASK_BTC_TASK_TYPES_H_

namespace task {

typedef std::vector<uint8_t> BlockBuffer_t;
typedef std::vector<uint8_t> TxBuffer_t;
typedef utils::crypto::Sha256Map_t<unsigned int> Sha256ToShortID_t;
typedef std::unordered_map<unsigned int, utils::crypto::Sha256> ShortIDToSha256Map_t;
typedef utils::crypto::Sha256Map_t<TxBuffer_t> Sha256ToTxMap_t;
typedef std::list<std::pair<size_t, size_t>> TXOffsets_t;
typedef std::vector<const utils::crypto::Sha256*> UnknownTxHashes_t;
typedef std::vector<unsigned int> UnknownTxSIDs_t;

} // task

#endif /* TPE_TASK_BTC_TASK_TYPES_H_ */
