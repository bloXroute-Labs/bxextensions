#include <utility>

#include "tpe/task/sub_task/btc_block_decompression_sub_task.h"

namespace task {

BtcBlockDecompressionSubTask::BtcBlockDecompressionSubTask():
		SubTaskBase(),
		_tx_service(nullptr),
		_block_buffer(nullptr),
		_output_buffer(nullptr),
		_short_ids(nullptr)
{
}

void BtcBlockDecompressionSubTask::init(
		PTransactionService_t tx_service,
		const BlockBuffer_t* block_buffer,
		utils::common::ByteArray* output_buffer,
		const ShortIDs_t* short_ids
)
{
	_tx_service = std::move(tx_service);
	_block_buffer = block_buffer;
	_output_buffer = output_buffer;
	_short_ids = short_ids;
}

BtcBlockDecompressionSubTask::TaskData&
BtcBlockDecompressionSubTask::task_data() {
	return _task_data;
}

void BtcBlockDecompressionSubTask::_execute() {
	size_t short_id_idx = _task_data.short_ids_offset;
	size_t output_offset = _task_data.output_offset;
	for (auto& offsets : *_task_data.offsets) {
		service::PTxContents_t content = nullptr;
        const service::TxContents_t* p_contents = nullptr;
        size_t from = std::get<0>(offsets);
        size_t to = std::get<2>(offsets);
		size_t size = to - from;
		if (size == 1) {
			unsigned int short_id = _short_ids->at(short_id_idx);
            p_contents = _tx_service->get_tx_contents_raw_ptr(short_id);
			short_id_idx += 1;
		} else {
			content = std::make_shared<BufferView_t>(
					*_block_buffer,
					size,
					from
			);
            p_contents = content.get();
		}
		output_offset = _output_buffer->copy_from_buffer(
				*p_contents,
				output_offset,
				0,
                p_contents->size()
		);
	}


	_tx_service = nullptr;
	_block_buffer = nullptr;
	_output_buffer = nullptr;
	_short_ids = nullptr;
}

} // task
