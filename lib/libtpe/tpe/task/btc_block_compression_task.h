#include <iostream>
#include <memory>
#include <list>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>
#include <utils/protocols/bitcoin/btc_block_message.h>

#include "tpe/task/sub_task/btc_block_compression_sub_task.h"
#include "tpe/task/main_task_base.h"
#include "tpe/consts.h"


#ifndef TPE_TASK_BTC_BLOCK_COMPRESSION_TASK_H_
#define TPE_TASK_BTC_BLOCK_COMPRESSION_TASK_H_

namespace task {

class BTCBlockCompressionTask : public MainTaskBase {
	typedef std::shared_ptr<BTCBlockCompressionSubTask> PSubTask_t;

	struct TaskData {
		TaskData(): sub_task(nullptr), offsets(nullptr) {}
		TaskData(TaskData&& other) {
			sub_task = std::move(other.sub_task);
			offsets = std::move(other.offsets);
		}

		PSubTask_t sub_task;
		std::shared_ptr<TXOffsets_t> offsets;
	};

	typedef std::vector<TaskData> SubTasksData_t;

public:
	BTCBlockCompressionTask(
			size_t capacity = BTC_DEFAULT_BLOCK_SIZE,
			size_t minimal_tx_count = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
	);

	void init(
			BlockBuffer_t block_buffer,
			PTransactionService_t tx_service
	);

	const utils::common::ByteArray& bx_block(void);

	const utils::crypto::Sha256& prev_block_hash(void);
	const utils::crypto::Sha256& block_hash(void);
	const utils::crypto::Sha256& compressed_block_hash(void);

	size_t txn_count(void);

	const std::vector<unsigned int>& short_ids(void);

protected:
	void _execute(SubPool_t& sub_pool) override;

private:
	void _init_sub_tasks(size_t pool_size, size_t tx_count);
	size_t _dispatch(
			size_t tx_count,
			utils::protocols::BTCBlockMessage& msg,
			size_t offset,
			SubPool_t& sub_pool
	);
	size_t _on_sub_task_completed(BTCBlockCompressionSubTask& tsk);
	void _set_output_buffer(size_t output_offset);
	void _enqueue_task(size_t task_idx, SubPool_t& sub_pool);



	BlockBuffer_t _block_buffer;
	utils::common::ByteArray _output_buffer;
	PTransactionService_t _tx_service;
	SubTasksData_t _sub_tasks;
	const size_t _minimal_tx_count;
	utils::crypto::Sha256 _prev_block_hash, _block_hash;
	utils::crypto::Sha256 _compressed_block_hash;
	size_t _txn_count;
	std::vector<unsigned int> _short_ids;
};

} // task



#endif /* TPE_TASK_BTC_BLOCK_COMPRESSION_TASK_H_ */
