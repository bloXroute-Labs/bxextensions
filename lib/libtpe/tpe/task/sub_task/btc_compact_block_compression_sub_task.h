#include <iostream>
#include <vector>
#include <memory>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/service/compact_block_data_service.h"
#include "tpe/task/btc_task_types.h"

#ifndef TPE_TASK_SUB_TASK_BTC_COMPACT_BLOCK_COMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_BTC_COMPACT_BLOCK_COMPRESSION_SUB_TASK_H_

namespace task {

typedef service::CompactTransactionPlaceholders_t CompactTransactionPlaceholders_t;
typedef std::shared_ptr<TxBuffer_t> PTxBuffer_t;
typedef std::vector<PTxBuffer_t> RecoveredTransactions_t;

class BtcCompactBlockCompressionSubTask : public SubTaskBase {
public:

	BtcCompactBlockCompressionSubTask(
			const CompactTransactionPlaceholders_t& tx_placeholders,
			const RecoveredTransactions_t& recovered_transactions,
			const std::vector<uint8_t>& block_header
	);

	uint32_t payload_length();
	uint32_t checksum();

protected:

	void _execute(void) override;

private:

	const CompactTransactionPlaceholders_t& _tx_placeholders;
	const RecoveredTransactions_t& _recovered_transactions;
	const std::vector<uint8_t>& _block_header;
	uint32_t _payload_length;
	uint32_t _checksum;
};

} // task

#endif /* TPE_TASK_SUB_TASK_BTC_COMPACT_BLOCK_COMPRESSION_SUB_TASK_H_ */
