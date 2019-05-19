#include <utils/common/buffer_helper.h>
#include <utils/crypto/hash_helper.h>
#include <algorithm>

#include "tpe/task/btc_block_compression_task.h"

namespace task {

BTCBlockCompressionTask::BTCBlockCompressionTask(
		size_t capacity/* = BTC_DEFAULT_BLOCK_SIZE*/,
		size_t minimal_tx_count/* = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/

):
				MainTaskBase(),
				_tx_service(nullptr),
				_output_buffer(capacity),
				_minimal_tx_count(minimal_tx_count),
				_txn_count(0)
{
}

void BTCBlockCompressionTask::init(
		BlockBuffer_t block_buffer,
		PTransactionService_t tx_service
)
{
	_tx_service = tx_service;
	_block_buffer = block_buffer;
	_output_buffer.reset();
	_output_buffer.reserve(block_buffer.size());
	_short_ids.clear();
}

const utils::common::ByteArray &
BTCBlockCompressionTask::bx_block() {
	_assert_execution();
	return _output_buffer;
}

const utils::crypto::Sha256&
BTCBlockCompressionTask::prev_block_hash() {
	_assert_execution();
	return _prev_block_hash;
}

const utils::crypto::Sha256&
BTCBlockCompressionTask::block_hash() {
	_assert_execution();
	return _block_hash;
}

const utils::crypto::Sha256&
BTCBlockCompressionTask::compressed_block_hash() {
	_assert_execution();
	return _compressed_block_hash;
}

size_t BTCBlockCompressionTask::txn_count() {
	return _txn_count;
}

const std::vector<unsigned int>&
BTCBlockCompressionTask::short_ids() {
	_assert_execution();
	return _short_ids;
}

void BTCBlockCompressionTask::_execute(SubPool_t& sub_pool) {
	utils::protocols::BTCBlockMessage msg(_block_buffer);
	_prev_block_hash = std::move(msg.prev_block_hash());
	_block_hash = std::move(msg.block_hash());
	uint64_t tx_count = 0;
	size_t offset = msg.get_tx_count(tx_count);
	_txn_count = tx_count;
	size_t last_idx = _dispatch(tx_count, msg, offset, sub_pool);
	size_t output_offset = sizeof(size_t);
	output_offset = _output_buffer.copy_from_buffer(
			_block_buffer,
			output_offset,
			0,
			offset
	);
	for (size_t idx = 0 ; idx <= last_idx ; ++idx) {
		auto& tdata = _sub_tasks[idx];
		tdata.sub_task->wait();
		output_offset = _on_sub_task_completed(*tdata.sub_task);
	}
	_set_output_buffer(output_offset);
	_compressed_block_hash = std::move(
			utils::crypto::double_sha256(
					_output_buffer.array(),
					0,
					_output_buffer.size()
			)
	);
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

size_t BTCBlockCompressionTask::_on_sub_task_completed(
		BTCBlockCompressionSubTask& tsk
)
{
	auto& output_buffer = tsk.output_buffer();
	auto& short_ids = tsk.short_ids();
	_output_buffer += output_buffer;
	_short_ids.reserve(_short_ids.size() + short_ids.size());
	_short_ids.insert(
			_short_ids.end(),
			short_ids.begin(),
			short_ids.end()
	);
	return _output_buffer.size();
}

void BTCBlockCompressionTask::_set_output_buffer(
		size_t output_offset
)
{
	utils::common::set_little_endian_value(
			_output_buffer,
			output_offset,
			0
	);
	const unsigned int short_ids_size = _short_ids.size();
	output_offset = utils::common::set_little_endian_value(
			_output_buffer,
			short_ids_size,
			output_offset
	);
	if (short_ids_size > 0) {
		const size_t short_ids_byte_size  =
				short_ids_size * sizeof(unsigned int);
		_output_buffer.resize(output_offset + short_ids_byte_size);
		memcpy(
				&_output_buffer[output_offset],
				(unsigned char*) &_short_ids.at(0),
				short_ids_byte_size
		);
	}
	_output_buffer.set_output();
}

void BTCBlockCompressionTask::_enqueue_task(
		size_t task_idx,
		SubPool_t& sub_pool
)
{
	TaskData& tdata = _sub_tasks[task_idx];
	tdata.sub_task->init(
			_tx_service,
			&_block_buffer,
			tdata.offsets
	);
	sub_pool.enqueue_task(tdata.sub_task);
}

} // task
