#include <iostream>
#include <cstdint>
#include <list>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/task/btc_task_types.h"


#ifndef TPE_TASK_SUB_TASK_BTC_BLOCK_COMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_BTC_BLOCK_COMPRESSION_SUB_TASK_H_

namespace task {

typedef std::shared_ptr<TransactionService_t> PTransactionService_t;

class BTCBlockCompressionSubTask : public SubTaskBase {

public:
	BTCBlockCompressionSubTask(size_t capacity);

	void init(
			PTransactionService_t tx_service,
			const BlockBuffer_t* block_buffer,
			POffests_t tx_offsets
	);

	const utils::common::ByteArray& output_buffer(void);
	const std::vector<unsigned int>& short_ids(void);


protected:
	void _execute(void) override;

private:

	utils::common::ByteArray _output_buffer;
	const BlockBuffer_t* _block_buffer;
	POffests_t _tx_offsets;
	PTransactionService_t _tx_service;
	std::vector<unsigned int> _short_ids;
};

}



#endif /* TPE_TASK_SUB_TASK_BTC_BLOCK_COMPRESSION_SUB_TASK_H_ */
