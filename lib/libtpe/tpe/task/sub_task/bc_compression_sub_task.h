#include <iostream>
#include <cstdint>
#include <list>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>

#include "tpe/task/sub_task/sub_task_base.h"
#include "utils/protocols/bitcoin/btc_consts.h"

#ifndef TPE_TASK_SUB_TASK_BC_COMPRESSION_SUB_TASK_H_
#define TPE_TASK_SUB_TASK_BC_COMPRESSION_SUB_TASK_H_

typedef std::vector<uint8_t> BlockBuffer_t;
typedef utils::crypto::Sha256Map_t<unsigned int> Sha256ToShortID_t;
typedef std::list<std::pair<size_t, size_t>> TXOffsets_t;

namespace task {

class BCCompressionSubTask : public SubTaskBase {
public:
	BCCompressionSubTask(
			const Sha256ToShortID_t& short_id_map,
			size_t capacity);

	void init(
			const BlockBuffer_t* block_buffer,
			const TXOffsets_t* tx_offsets
	);

	const utils::common::ByteArray& output_buffer(void) const;


protected:
	void _execute(void) override;

private:

	utils::common::ByteArray _output_buffer;
	const BlockBuffer_t* _block_buffer;
	const TXOffsets_t* _tx_offsets;
	const Sha256ToShortID_t& _short_id_map;
};

}



#endif /* TPE_TASK_SUB_TASK_BC_COMPRESSION_SUB_TASK_H_ */
