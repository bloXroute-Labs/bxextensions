#include <iostream>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>

#include "tpe/task/main_task_base.h"
#include "tpe/task/sub_task/bc_compression_sub_task.h"

#ifndef TPE_TASK_BC_COMPRESSION_TASK_H_
#define TPE_TASK_BC_COMPRESSION_TASK_H_

namespace task {

class BCCompressionTask : public MainTaskBase {
	typedef std::shared_ptr<BCCompressionSubTask> PSubTask_t;
	typedef std::vector<PSubTask_t> SubTasks_t;
public:
	BCCompressionTask(const Sha256ToShortID_t& short_id_map,
			size_t capacity = BTC_DEFAULT_BLOCK_SIZE);

	void init(const BlockBuffer_t& block_buffer);

	const std::vector<unsigned short>& bx_buffer(void);

protected:
	void _execute(SubPool_t& sub_pool) override;
private:

	BlockBuffer_t _block_buffer;
	utils::common::ByteArray _output_buffer;
	const Sha256ToShortID_t& _short_id_map;
	SubTasks_t _sub_tasks;
};

} // task



#endif /* TPE_TASK_BC_COMPRESSION_TASK_H_ */
