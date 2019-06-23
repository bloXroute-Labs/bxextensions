#include <utility>

#include <unistd.h>
#include <algorithm>

#include "tpe/task/btc_compact_block_mapping_task.h"
#include "tpe/service/compact_block_data_service.h"

namespace task {

typedef service::CompactBlockDataService CompactBlockDataService_t;


BtcCompactBlockMappingTask::BtcCompactBlockMappingTask(size_t capacity/* = BTC_DEFAULT_BLOCK_SIZE*/):
		MainTaskBase(),
		_block_buffer(nullptr),
        _tx_service(nullptr),
		_magic(0),
		_capacity(capacity)
{
	_missing_indices = std::make_shared<UnknownTxIndices_t>();
	_compression_task = std::make_shared<BtcCompactBlockCompressionTask>(capacity);
}

void BtcCompactBlockMappingTask::init(
		PBlockBuffer_t block_buffer,
		PTransactionService_t tx_service,
		uint32_t magic
)
{
	if (_missing_indices.use_count() > 1) {
		_missing_indices = std::make_shared<UnknownTxIndices_t>();
	} else {
		_missing_indices->clear();
	}
	_block_buffer = std::move(block_buffer);
	_tx_service = std::move(tx_service);
	_magic = magic;
	if (_compression_task.use_count() > 1) {
		_compression_task = std::make_shared<BtcCompactBlockCompressionTask>(
				std::max(_block_buffer->size(), _capacity)
		);
	}
}

bool BtcCompactBlockMappingTask::success() {
	assert_execution();
	return _missing_indices->empty();
}

PUnknownTxIndices_t
BtcCompactBlockMappingTask::missing_indecies() {
	assert_execution();
	return _missing_indices;
}

PCompressionTask_t BtcCompactBlockMappingTask::compression_task() {
	assert_execution();
	return _compression_task;
}

void BtcCompactBlockMappingTask::_execute(SubPool_t& sub_pool) {
	CompactBlockDataService_t service(
			_block_buffer, &_tx_service->tx_hash_to_short_ids()
	);
	service.parse();
	uint64_t compact_tx_count = service.compact_tx_count();
	CompactShortIdsCounter_t counter(0);
	const CompactShortIdToShortIdMap_t& compact_map = service.compact_map();
	const CompactTransactionPlaceholders_t& tx_placeholders = service.tx_placeholders();
	const TxNotSeenInBlocks_t& tx_container = _tx_service->acquire_tx_pool();
	int task_idx = _dispatch(
			sub_pool,
			tx_container,
			service.compact_map(),
			service.tx_placeholders(),
			counter,
			service.sip_key()
	);

	for(int idx = 0 ; idx <= task_idx ; ++idx) {
		_sub_tasks[idx]->wait();
	}
	size_t compact_found_count = counter.load();
	if (compact_found_count < compact_tx_count) {
		for (const auto & compact_iter : compact_map)
		{
			const auto& pair = compact_iter.second;
			const auto& placeholder = tx_placeholders.at(pair.second);
			if (placeholder.type == TransactionPlaceholderType_t::missing_compact_id) {
				_missing_indices->push_back(pair.second);
			}
		}
		std::sort(_missing_indices->begin(), _missing_indices->end());
	}
	size_t total_compact_processed =
			_missing_indices->size() + compact_found_count;
	if (total_compact_processed != compact_tx_count) {
		throw std::runtime_error("invalid compact short id processing");
        // TODO : replace with proper exception here
	}
	_compression_task->init(std::move(service), _magic);
	_tx_service->on_finished_reading_tx_pool();
	_block_buffer = nullptr;
	_tx_service = nullptr;
}

void BtcCompactBlockMappingTask::_init_sub_tasks(
		const TxNotSeenInBlocks_t& tx_container
)
{
	uint32_t bucket_count = tx_container.bucket_count();
	uint32_t task_count = _sub_tasks.size();
	for (uint32_t idx = task_count ; idx < bucket_count ; ++idx) {
		_sub_tasks.push_back(
				std::make_shared<BtcCompactTxMappingSubTask>()
		);
	}
}

int BtcCompactBlockMappingTask::_dispatch(
		SubPool_t& sub_pool,
		const TxNotSeenInBlocks_t& tx_container,
		CompactShortIdToShortIdMap_t& compact_map,
		CompactTransactionPlaceholders_t& transaction_placeholders,
		CompactShortIdsCounter_t& counter,
		const SipKey_t& key
)
{
	_init_sub_tasks(tx_container);
	auto sub_task_iter = _sub_tasks.begin();
	int task_idx = -1;
	for(uint32_t idx = 0 ; idx < tx_container.bucket_count() ; ++idx)
	{
		auto& bucket = tx_container[idx];
		if (
				bucket.empty() &&
				(idx < tx_container.bucket_count() - 1 ||
				task_idx >= 0)
		)
		{
			continue;
		}
		auto& sub_task = *sub_task_iter;
		sub_task->init(
				&compact_map,
				_missing_indices.get(),
				_tx_service.get(),
				&bucket,
				&transaction_placeholders,
				&counter,
				&key
		);
		sub_pool.enqueue_task(sub_task);

		++sub_task_iter;
		++task_idx;
	}
	return task_idx;
}


} // task
