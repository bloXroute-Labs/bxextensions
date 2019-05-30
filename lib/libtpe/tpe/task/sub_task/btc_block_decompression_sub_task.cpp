#include "tpe/task/sub_task/btc_block_decompression_sub_task.h"

namespace task {

BTCBlockDecompressionSubTask::BTCBlockDecompressionSubTask():
		_tx_service(nullptr),
		_block_buffer(nullptr),
		_output_buffer(nullptr),
		_short_ids(nullptr)
{
}

void BTCBlockDecompressionSubTask::init(
		PTransactionService_t tx_service,
		const BlockBuffer_t* block_buffer,
		utils::common::ByteArray* output_buffer,
		const ShortIDs_t* short_ids
)
{
	_tx_service = tx_service;
	_block_buffer = block_buffer;
	_output_buffer = output_buffer;
	_short_ids = short_ids;
}

BTCBlockDecompressionSubTask::TaskData&
BTCBlockDecompressionSubTask::task_data() {
	return _task_data;
}

void BTCBlockDecompressionSubTask::_execute() {
	size_t short_id_idx = _task_data.short_ids_offset;
	size_t output_offset = _task_data.output_offset;
	for (auto& pair : *_task_data.offsets) {
		service::TxContents_t content = nullptr;
		const size_t from = pair.first, to = pair.second;
		size_t size = to - from;
		if (size == 1) {
			unsigned int short_id = _short_ids->at(short_id_idx);
			utils::crypto::Sha256 tx_hash;
			content = _tx_service->get_transaction(short_id, tx_hash);
			short_id_idx += 1;
		} else {
			content = std::make_shared<BufferView_t>(
					*_block_buffer,
					size,
					from
			);
		}
		output_offset = _output_buffer->copy_from_buffer(
				*content,
				output_offset,
				0,
				content->size()
		);
	}


	_tx_service = nullptr;
	_block_buffer = nullptr;
	_output_buffer = nullptr;
	_short_ids = nullptr;
}

} // task
