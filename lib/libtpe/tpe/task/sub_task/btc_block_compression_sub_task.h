#include <iostream>
#include <cstdint>
#include <list>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "tpe/task/btc_task_types.h"

#ifndef TPE_TASK_SUB_TASK_BTC_BLOCK_COMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_BTC_BLOCK_COMPRESSION_SUB_TASK_H_

namespace task {

class BTCBlockCompressionSubTask : public SubTaskBase {
	typedef std::shared_ptr<const BlockBuffer_t> PBuffer_t;
	typedef std::shared_ptr<const TXOffsets_t> POffests_t;

public:
	BTCBlockCompressionSubTask(size_t capacity);

	void init(
			const Sha256ToShortID_t* short_id_map,
			PBuffer_t block_buffer,
			POffests_t tx_offsets
	);

	const utils::common::ByteArray& output_buffer(void);
	const std::vector<unsigned int>& short_ids(void);


protected:
	void _execute(void) override;

private:

	utils::common::ByteArray _output_buffer;
	PBuffer_t _block_buffer;
	POffests_t _tx_offsets;
	const Sha256ToShortID_t* _short_id_map;
	std::vector<unsigned int> _short_ids;
};

}



#endif /* TPE_TASK_SUB_TASK_BTC_BLOCK_COMPRESSION_SUB_TASK_H_ */
