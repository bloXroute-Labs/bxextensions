#include <iostream>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>
#include <utils/protocols/bitcoin/btc_block_message.h>

#include "tpe/task/sub_task/btc_block_compression_sub_task.h"
#include "tpe/task/main_task_base.h"

#ifndef TPE_TASK_BTC_BLOCK_COMPRESSION_TASK_H_
#define TPE_TASK_BTC_BLOCK_COMPRESSION_TASK_H_

namespace task {

class BTCBlockCompressionTask : public MainTaskBase {
	typedef std::shared_ptr<BTCBlockCompressionSubTask> PSubTask_t;

	struct TaskData {
		PSubTask_t sub_task;
		TXOffsets_t offsets;
	};

	typedef std::vector<TaskData> SubTasksData_t;

public:
	BTCBlockCompressionTask(const Sha256ToShortID_t& short_id_map,
			size_t capacity = BTC_DEFAULT_BLOCK_SIZE);

	void init(const BlockBuffer_t& block_buffer);

	const std::vector<unsigned short>& bx_block(void);

protected:
	void _execute(SubPool_t& sub_pool) override;

private:
	void _init_sub_tasks(size_t pool_size, size_t tx_count);
	void _dispatch(
			size_t tx_count,
			utils::protocols::BTCBlockMessage& msg,
			size_t offset,
			SubPool_t& sub_pool
	);
	void _enqueue_task(size_t task_idx, SubPool_t& sub_pool);


	BlockBuffer_t _block_buffer;
	utils::common::ByteArray _output_buffer;
	const Sha256ToShortID_t& _short_id_map;
	SubTasksData_t _sub_tasks;
};

} // task



#endif /* TPE_TASK_BTC_BLOCK_COMPRESSION_TASK_H_ */
