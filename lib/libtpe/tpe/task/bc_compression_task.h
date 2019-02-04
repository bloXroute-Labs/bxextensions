#include <iostream>

#include <utils/common/byte_array.h>
#include <utils/crypto/sha256.h>

#include "tpe/task/main_task_base.h"
#include "tpe/consts.h"

#ifndef TPE_TASK_BC_COMPRESSION_TASK_H_
#define TPE_TASK_BC_COMPRESSION_TASK_H_

typedef std::vector<uint8_t> BlockBuffer_t;
typedef utils::crypto::Sha256Map_t<unsigned int> Sha256ToShortID_t;

namespace task {

class BCCompressionTask : public MainTaskBase {

public:
	BCCompressionTask(const Sha256ToShortID_t& short_id_map,
			size_t capacity = BC_DEFAULT_BLOCK_SIZE);

	void init(const BlockBuffer_t& block_buffer, size_t header_offset);

protected:
	void _execute(SubPool_t& sub_pool) override;
private:

	BlockBuffer_t _block_buffer;
	size_t _header_offset;
	utils::common::ByteArray _output_buffer;
	const Sha256ToShortID_t& _short_id_map;
};

} // task



#endif /* TPE_TASK_BC_COMPRESSION_TASK_H_ */
