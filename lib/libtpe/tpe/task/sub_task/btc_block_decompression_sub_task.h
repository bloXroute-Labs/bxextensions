#include <iostream>
#include <cstdint>
#include <list>
#include <vector>
#include <memory>

#include <utils/common/byte_array.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/task/btc_task_types.h"


#ifndef TPE_TASK_SUB_TASK_BTC_BLOCK_DECOMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_BTC_BLOCK_DECOMPRESSION_SUB_TASK_H_

namespace task {

typedef std::vector<unsigned int> ShortIDs_t;
typedef utils::common::BufferView BufferView_t;

class BtcBlockDecompressionSubTask : public SubTaskBase {
public:

	struct TaskData {
		TaskData():
			short_ids_offset(0),
			short_ids_len(0),
			output_offset(0)
		{
			offsets = std::make_shared<TXOffsets_t>();
		}

		void clear() {
			short_ids_offset = 0;
			short_ids_len = 0;
			output_offset = 0;
			offsets->clear();
		}

		POffests_t offsets;
		size_t short_ids_offset;
		size_t short_ids_len;
		size_t output_offset;
	};

	BtcBlockDecompressionSubTask();

	void init(
			PTransactionService_t tx_service,
			const BlockBuffer_t* block_buffer,
			utils::common::ByteArray* output_buffer,
			const ShortIDs_t* short_ids
	);

	TaskData& task_data();

protected:
	void _execute() override;

private:
	PTransactionService_t _tx_service;
	const BlockBuffer_t* _block_buffer;
	utils::common::ByteArray* _output_buffer;
	const ShortIDs_t* _short_ids;
	TaskData _task_data;
};

} // task

#endif /* TPE_TASK_SUB_TASK_BTC_BLOCK_DECOMPRESSION_SUB_TASK_H_ */
