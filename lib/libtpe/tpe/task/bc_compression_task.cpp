#include <utils/common/buffer_helper.h>
#include <utils/crypto/hash_helper.h>

#include "tpe/task/bc_compression_task.h"

namespace task {

BCCompressionTask::BCCompressionTask(
		const Sha256ToShortID_t& short_id_map,
		size_t capacity/* = BC_DEFAULT_BLOCK_SIZE*/):
				_short_id_map(short_id_map),
				_output_buffer(capacity),
				_header_offset(0) {
	_block_buffer.reserve(capacity);

}

void BCCompressionTask::init(
		const BlockBuffer_t& block_buffer,
		size_t header_offset) {
	_block_buffer = block_buffer;
	_header_offset = header_offset;
}

void BCCompressionTask::_execute(SubPool_t& sub_pool) {
	_output_buffer.from_char_array((char *)&_block_buffer[0],
			_header_offset);
	size_t offset = BC_TX_COUNT_OFFSET + 1;
	size_t tx_count = utils::common::get_unsigned_long_int(
			_block_buffer,
			offset,
			BC_TX_COUNT_OFFSET
	);
	for (int count = 0 ; count < tx_count ; ++count) {
		utils::crypto::Sha256 sha = utils::crypto::double_sha256(
			_block_buffer,
			offset,
			offset + 10 // TODO : continue implementation by replacing this line
		);
	}
}

} // task
