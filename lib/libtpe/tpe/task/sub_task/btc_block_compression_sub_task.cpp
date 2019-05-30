#include <utils/common/buffer_helper.h>
#include <utils/protocols/bitcoin/btc_block_message.h>
#include <utils/crypto/hash_helper.h>

#include "tpe/consts.h"
#include "tpe/task/sub_task/btc_block_compression_sub_task.h"



namespace task {

BTCBlockCompressionSubTask::BTCBlockCompressionSubTask(
		size_t capacity):
				SubTaskBase(),
				_tx_service(nullptr),
				_output_buffer(capacity),
				_tx_offsets(nullptr),
				_block_buffer(nullptr)
{
}

void BTCBlockCompressionSubTask::init(
		PTransactionService_t tx_service,
		const BlockBuffer_t* block_buffer,
		POffests_t tx_offsets
)
{
	_tx_service = tx_service;
	_block_buffer = block_buffer;
	_tx_offsets = tx_offsets;
	_short_ids.clear();
	_output_buffer.reset();
}

const utils::common::ByteArray&
BTCBlockCompressionSubTask::output_buffer() {
	assert_execution();
	return _output_buffer;
}

const std::vector<unsigned int>&
BTCBlockCompressionSubTask::short_ids() {
	assert_execution();
	return _short_ids;
}

void BTCBlockCompressionSubTask::_execute()  {
	size_t output_offset = 0;
	for (auto& pair : *_tx_offsets) {
		size_t from = pair.first, offset = pair.second;
		const utils::crypto::Sha256 sha = std::move(
				utils::crypto::double_sha256(
						*_block_buffer,
						from,
						offset - from
		));
		if (_tx_service->has_short_id(sha)) {
			unsigned int short_id = _tx_service->get_short_id(sha);
			uint8_t flag = BTC_SHORT_ID_INDICATOR;
			output_offset =
					utils::common::set_little_endian_value<uint8_t>(
					_output_buffer,
					flag,
					output_offset);
			_short_ids.push_back(short_id);
		} else {
			output_offset = _output_buffer.copy_from_buffer(
					*_block_buffer,
					output_offset,
					from,
					offset - from
			);
		}
	}

	_tx_service = nullptr;
	_tx_offsets = nullptr;
	_block_buffer = nullptr;
}

}
