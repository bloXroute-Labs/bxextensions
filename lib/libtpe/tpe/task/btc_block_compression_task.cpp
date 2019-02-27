#include <utils/common/buffer_helper.h>
#include <utils/crypto/hash_helper.h>

#include "tpe/task/btc_block_compression_task.h"

namespace task {

BTCBlockCompressionTask::BTCBlockCompressionTask(
		size_t capacity/* = BTC_DEFAULT_BLOCK_SIZE*/,
		size_t minimal_tx_count/* = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/

):
				MainTaskBase(),
				_short_id_map(nullptr),
				_output_buffer(capacity),
				_minimal_tx_count(minimal_tx_count),
				_txn_count(0)
{
	_block_buffer = std::make_shared<BlockBuffer_t>();
	_block_buffer->reserve(capacity);
}

void BTCBlockCompressionTask::init(
		const BlockBuffer_t& block_buffer,
		const Sha256ToShortID_t* short_id_map
)
{
	_short_id_map = short_id_map;
	_block_buffer->assign(block_buffer.begin(), block_buffer.end());
	_output_buffer.reset();
}

const utils::common::ByteArray & BTCBlockCompressionTask::bx_block() {
	_assert_execution();
	return _output_buffer;
}

const utils::crypto::Sha256&
BTCBlockCompressionTask::prev_block_hash() const {
	return _prev_block_hash;
}

const utils::crypto::Sha256&
BTCBlockCompressionTask::block_hash() const {
	return _block_hash;
}

const utils::crypto::Sha256&
BTCBlockCompressionTask::compressed_block_hash() const {
	return _compressed_block_hash;
}

size_t BTCBlockCompressionTask::txn_count() const {
	return _txn_count;
}

void BTCBlockCompressionTask::_execute(SubPool_t& sub_pool) {
	utils::protocols::BTCBlockMessage msg(*_block_buffer);
	_prev_block_hash = std::move(msg.prev_block_hash());
	_block_hash = std::move(msg.block_hash());
	uint64_t tx_count = 0;
	size_t offset = msg.get_tx_count(tx_count), output_offset = 0;
	_txn_count = tx_count;
	size_t last_idx = _dispatch(tx_count, msg, offset, sub_pool);
	output_offset = _output_buffer.copy_from_array(
			*_block_buffer,
			output_offset,
			0,
			offset
	);
	for (size_t idx = 0 ; idx <= last_idx ; ++idx) {
		auto& tdata = _sub_tasks[idx];
		tdata.sub_task->wait();
		_output_buffer += tdata.sub_task->output_buffer();
	}
	_output_buffer.set_output();
	_compressed_block_hash = std::move(
			utils::crypto::double_sha256(
					_output_buffer.array(),
					0,
					_output_buffer.size()
			));
}

void BTCBlockCompressionTask::_init_sub_tasks(
		size_t pool_size,
		size_t tx_count
)
{
	if (_sub_tasks.size() < pool_size) {
		size_t default_count = BTC_DEFAULT_TX_COUNT;
		size_t capacity = std::max(
				default_count,
				(size_t) (tx_count / pool_size)
		) * BTC_DEFAULT_TX_SIZE;
		for (
				size_t i = _sub_tasks.size() ;
				i < pool_size ;
				++i
		)
		{
			TaskData task_data;
			task_data.sub_task = std::make_shared<
					BTCBlockCompressionSubTask>(
					capacity
			);
			task_data.offsets = std::make_shared<TXOffsets_t>();
			_sub_tasks.push_back(std::move(task_data));
		}
	} else {
		for (auto& task_data : _sub_tasks) {
			task_data.offsets->clear();
		}
	}
}

size_t BTCBlockCompressionTask::_dispatch(
		size_t tx_count,
		utils::protocols::BTCBlockMessage& msg,
		size_t offset,
		SubPool_t& sub_pool
)
{
	size_t pool_size = sub_pool.size(), prev_idx = 0;
	_init_sub_tasks(pool_size, tx_count);
	size_t bulk_size = std::max(
			(size_t) (tx_count / pool_size),
			std::max(pool_size, _minimal_tx_count)
	);
	size_t idx = 0;
	for (int count = 0 ; count < tx_count ; ++count) {
		size_t from = offset;
		idx = std::min((size_t) (count / bulk_size), pool_size - 1);
		offset = msg.get_next_tx_offset(offset);
		_sub_tasks[idx].offsets->push_back(
				std::pair<size_t, size_t>(from, offset));
		if(idx > prev_idx) {
			_enqueue_task(prev_idx, sub_pool);
		}
		prev_idx = idx;
	}
	_enqueue_task(prev_idx, sub_pool);
	return prev_idx;
}

void BTCBlockCompressionTask::_enqueue_task(
		size_t task_idx,
		SubPool_t& sub_pool
)
{
	TaskData& tdata = _sub_tasks[task_idx];
	tdata.sub_task->init(
			_short_id_map,
			_block_buffer,
			tdata.offsets
	);
	sub_pool.enqueue_task(tdata.sub_task);
}

} // task
