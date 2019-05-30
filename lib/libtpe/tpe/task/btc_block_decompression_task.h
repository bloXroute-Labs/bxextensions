#include <iostream>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/common/buffer_view.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>
#include <utils/protocols/bitcoin/bx_btc_block_message.h>

#include "tpe/task/main_task_base.h"
#include "tpe/task/btc_task_types.h"
#include "tpe/task/sub_task/btc_block_decompression_sub_task.h"
#include "tpe/consts.h"

#ifndef TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_
#define TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_

namespace task {

typedef std::shared_ptr<BTCBlockDecompressionSubTask> PSubTask_t;

class BTCBlockDecompressionTask : public MainTaskBase {

public:
	BTCBlockDecompressionTask(
			size_t capacity = BTC_DEFAULT_BLOCK_SIZE,
			size_t minimal_tx_count = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
	);

	void init(
			BlockBuffer_t block_buffer,
			PTransactionService_t tx_service
	);

	PByteArray_t btc_block(void);
	const UnknownTxHashes_t& unknown_tx_hashes(void);
	const UnknownTxSIDs_t& unknown_tx_sids(void);
	PSha256_t block_hash(void);
	bool success(void);
	uint64_t tx_count(void);
	const std::vector<unsigned int>& short_ids(void);


protected:
	void _execute(SubPool_t& sub_pool) override;

private:
	void _init_sub_tasks(size_t pool_size);
	size_t _dispatch(
			utils::protocols::BxBtcBlockMessage& msg,
			size_t offset,
			SubPool_t& sub_pool
	);
	void _enqueue_task(size_t task_idx, SubPool_t& sub_pool);

	utils::protocols::BxBtcBlockMessage _parse_block_header(
			size_t& offset,
			uint64_t& tx_count
	);


	BlockBuffer_t _block_buffer;
	PByteArray_t _output_buffer;
	UnknownTxHashes_t _unknown_tx_hashes;
	UnknownTxSIDs_t _unknown_tx_sids;
	std::vector<unsigned int> _short_ids;
	PSha256_t _block_hash;
	PTransactionService_t _tx_service;
	std::vector<PSubTask_t> _sub_tasks;
	const size_t _minimal_tx_count;
	bool _success;
	uint64_t _tx_count;
};

} // task



#endif /* TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_ */
