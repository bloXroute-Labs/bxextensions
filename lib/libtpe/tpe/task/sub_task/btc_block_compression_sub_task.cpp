#include <utils/common/buffer_helper.h>
#include <utils/protocols/bitcoin/btc_block_message.h>
#include <utils/crypto/hash_helper.h>

#include "tpe/task/sub_task/btc_block_compression_sub_task.h"



namespace task {

BTCBlockCompressionSubTask::BTCBlockCompressionSubTask(
		const Sha256ToShortID_t& short_id_map,
		size_t capacity):
				_short_id_map(short_id_map),
				_output_buffer(capacity),
				_block_buffer(nullptr),
				_tx_offsets(nullptr)
{
}

void BTCBlockCompressionSubTask::init(
		const BlockBuffer_t* block_buffer,
		const TXOffsets_t* tx_offsets
)
{
	_block_buffer = block_buffer;
	_tx_offsets = tx_offsets;
	_output_buffer.reset();
}

const utils::common::ByteArray& BTCBlockCompressionSubTask::output_buffer() const {
	return _output_buffer;
}

void BTCBlockCompressionSubTask::_execute()  {
	size_t output_offset = 0;
	for (auto& pair : *_tx_offsets) {
		size_t from = pair.first, offset = pair.second;
		utils::crypto::Sha256 sha = std::move(
				utils::crypto::double_sha256(
						*_block_buffer,
						from,
						offset - from
		));

		auto shaItr = _short_id_map.find(sha);
		if (shaItr != _short_id_map.end()) {
			unsigned int short_id = shaItr->second;
			uint8_t flag = BTC_SHORT_ID_INDICATOR;
			output_offset =
					utils::common::set_little_endian_value<uint8_t>(
					_output_buffer,
					flag,
					output_offset);
			output_offset =
					utils::common::set_little_endian_value(
					_output_buffer,
					short_id,
					output_offset
			);
		} else {
			output_offset = _output_buffer.copy_from_array(
					*_block_buffer,
					output_offset,
					from,
					offset - from
			);
		}
	}
	_block_buffer = nullptr;
	_tx_offsets = nullptr;
}

}
