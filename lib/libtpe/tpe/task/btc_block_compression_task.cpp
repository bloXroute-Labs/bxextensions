#include <utils/common/buffer_helper.h>
#include <utils/protocols/bitcoin/btc_block_message.h>
#include <utils/crypto/hash_helper.h>

#include "tpe/task/btc_block_compression_task.h"

namespace task {

BTCBlockCompressionTask::BTCBlockCompressionTask(
		const Sha256ToShortID_t& short_id_map,
		size_t capacity/* = BTC_DEFAULT_BLOCK_SIZE*/):
				_short_id_map(short_id_map),
				_output_buffer(capacity) {
	_block_buffer.reserve(capacity);
}

void BTCBlockCompressionTask::init(
		const BlockBuffer_t& block_buffer) {
	_block_buffer = block_buffer;
}

const std::vector<unsigned short>& BTCBlockCompressionTask::bx_buffer() {
	return _output_buffer.array();
}

void BTCBlockCompressionTask::_execute(SubPool_t& sub_pool) {
	utils::protocols::BTCBlockMessage msg(_block_buffer);
	unsigned long long tx_count = 0;
	size_t offset = msg.get_tx_count(tx_count), output_offset = 0;
	size_t buffer_hdr_offset = offset;

	if (_sub_tasks.size() < sub_pool.size()) {
		size_t default_count = BTC_DEFAULT_TX_COUNT;
		size_t capacity = std::max(
				default_count,
				(size_t) (tx_count / sub_pool.size())
		);
		for (
				size_t i = _sub_tasks.size() ;
				i < sub_pool.size() ;
				++i
		)
		{
			_sub_tasks.push_back(
					std::make_shared<BTCBlockCompressionSubTask>(
							_short_id_map,
							capacity
					));
		}
	}
	size_t pool_size = _sub_tasks.size();
	std::vector<TXOffsets_t> offset_vec;
	offset_vec.resize(pool_size);

	for (int count = 0 ; count < tx_count ; ++count) {
		size_t from = offset, idx = (size_t) (count / pool_size) ;
		offset = msg.get_next_tx_offset(offset);
		offset_vec[idx].push_back(
				std::pair<size_t, size_t>(from, offset));
	}

	for (size_t i = 0 ; i < pool_size ; ++i) {
		_sub_tasks[i]->init(&_block_buffer, &offset_vec[i]);
		sub_pool.enqueue_item(_sub_tasks[i]);
	}

	output_offset = _output_buffer.copy_from_array(
			_block_buffer,
			output_offset,
			0,
			buffer_hdr_offset
	);
	for (auto& sub_task: _sub_tasks) {
		sub_task->wait();
		_output_buffer += sub_task->output_buffer();
	}
}

} // task
