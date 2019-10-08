#include "tpe/task/btc_compact_block_compression_task.h"
#include "tpe/task/sub_task/btc_compact_tx_mapping_sub_task.h"
#include "tpe/task/btc_task_types.h"

#ifndef TPE_TASK_BTC_COMPACT_BLOCK_MAPPING_TASK_H_
#define TPE_TASK_BTC_COMPACT_BLOCK_MAPPING_TASK_H_

namespace task {

typedef std::shared_ptr<BlockBuffer_t> PBlockBuffer_t;
typedef std::shared_ptr<BtcCompactTxMappingSubTask> PMappingSubTask_t;
typedef std::shared_ptr<BtcCompactBlockCompressionTask> PCompressionTask_t;
typedef service::TxNotSeenInBlocks_t TxNotSeenInBlocks_t;
typedef utils::crypto::SipKey_t SipKey_t;
typedef utils::crypto::CompactShortId CompactShortId_t;
typedef std::shared_ptr<UnknownTxIndices_t> PUnknownTxIndices_t;

class BtcCompactBlockMappingTask : public MainTaskBase {
public:

	explicit BtcCompactBlockMappingTask(size_t capacity = BTC_DEFAULT_BLOCK_SIZE);

	void init(
			PBlockBuffer_t block_buffer,
			PTransactionService_t tx_service,
			uint32_t magic
	);

	bool success();

	PUnknownTxIndices_t missing_indices();

	PCompressionTask_t compression_task();

    size_t get_task_byte_size() const override;

    void cleanup() override;

protected:
	void _execute(SubPool_t& sub_pool) override;

private:
	void _init_sub_tasks(
			const TxNotSeenInBlocks_t& tx_container
	);
	int _dispatch(
			SubPool_t& sub_pool,
			const TxNotSeenInBlocks_t& tx_container,
			CompactShortIdToShortIdMap_t& compact_map,
			CompactTransactionPlaceholders_t& transaction_placeholders,
			CompactShortIdsCounter_t& counter,
			const SipKey_t& key
	);

	PBlockBuffer_t _block_buffer;
	PTransactionService_t _tx_service;
	PUnknownTxIndices_t _missing_indices;
	std::vector<PMappingSubTask_t> _sub_tasks;
	int _magic;
	size_t _capacity;
	PCompressionTask_t _compression_task;
};

} // task

#endif /* TPE_TASK_BTC_COMPACT_BLOCK_MAPPING_TASK_H_ */
