#include <iostream>
#include <vector>
#include <cstdint>
#include <atomic>

#include <utils/protocols/bitcoin/btc_compact_block_message.h>

#include "utils/crypto/hash_helper.h"
#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/service/transaction_service.h"
#include "tpe/service/transaction_placeholder.h"


#ifndef TPE_TASK_SUB_TASK_BTC_COMPACT_TX_MAPPING_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_BTC_COMPACT_TX_MAPPING_SUB_TASK_H_

namespace task {

typedef service::TransactionService TransactionService_t;
typedef utils::crypto::CompactShortIdMap_t<std::pair<unsigned int, size_t>> CompactShortIdToShortIdMap_t;
typedef utils::crypto::CompactShortId CompactShortId_t;
typedef std::vector<uint64_t> UnknownTxIndices_t;
typedef utils::crypto::Sha256Bucket_t Sha256Bucket_t;
typedef std::atomic<uint64_t> CompactShortIdsCounter_t;
typedef service::Sha256ToShortIDsMap_t Sha256ToShortIDsMap_t;
typedef service::TransactionPlaceholder TransactionPlaceholder_t;
typedef service::TransactionPlaceholderType TransactionPlaceholderType_t;
typedef std::vector<TransactionPlaceholder_t> CompactTransactionPlaceholders_t;

class BtcCompactTxMappingSubTask : public SubTaskBase {
public:
	BtcCompactTxMappingSubTask();

	void init(
			CompactShortIdToShortIdMap_t* compact_short_id_map,
			UnknownTxIndices_t* unknown_indices,
			const TransactionService_t* tx_service,
			const Sha256Bucket_t* bucket,
			CompactTransactionPlaceholders_t* transaction_placeholders,
			CompactShortIdsCounter_t* counter,
			const utils::crypto::SipKey_t* key
	);

	static const uint32_t null_short_id; // TODO: Need to take this from the BtcCompactBlockMessage
protected:
	void _execute(void) override;

private:
	CompactShortIdToShortIdMap_t* _compact_short_id_map;
	UnknownTxIndices_t* _unknown_indices;
	const TransactionService_t* _tx_service;
	const Sha256Bucket_t* _bucket;
	CompactTransactionPlaceholders_t* _transaction_placeholders;
	CompactShortIdsCounter_t* _counter;
	const utils::crypto::SipKey_t* _key;
};

} // task

#endif /* TPE_TASK_SUB_TASK_BTC_COMPACT_TX_MAPPING_SUB_TASK_H_ */
