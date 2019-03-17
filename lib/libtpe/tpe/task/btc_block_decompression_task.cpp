#include "tpe/task/btc_block_decompression_task.h"


namespace task {

BTCBlockDecompressionTask::BTCBlockDecompressionTask(
		const ShortIDToSha256Map_t& short_id_map,
		const Sha256ToTxMap_t& tx_map,
		size_t capacity/* = BTC_DEFAULT_BLOCK_SIZE*/
) :
		_short_id_map(short_id_map),
		_tx_map(tx_map),
		_output_buffer(capacity),
		_block_hash(),
		_partial_block(false)
{
}

void BTCBlockDecompressionTask::init(
		const BlockBuffer_t& block_buffer
)
{
	_unknown_tx_hashes.clear();
	_output_buffer.reset();
	_unknown_tx_sids.clear();
	_block_buffer = block_buffer;
	_block_hash.clear();
	_partial_block = false;
}

BTCBlockDecompressionTask::BlockMessage_t BTCBlockDecompressionTask::block_message() {
	_assert_execution();
	return _output_buffer;
}

const UnknownTxHashes_t& BTCBlockDecompressionTask::unknown_tx_hashes() {
	_assert_execution();
	return _unknown_tx_hashes;
}

const UnknownTxSIDs_t& BTCBlockDecompressionTask::unknown_tx_sids() {
	_assert_execution();
	return _unknown_tx_sids;
}

const utils::crypto::Sha256& BTCBlockDecompressionTask::block_hash() {
	_assert_execution();
	return _block_hash;
}

void BTCBlockDecompressionTask::_execute(SubPool_t& sub_pool) {
	utils::protocols::BTCBlockMessage msg(_block_buffer);
	_block_hash = std::move(msg.block_hash());
	uint64_t tx_count;
	size_t buffer_size = _block_buffer.size();
	size_t offset = msg.get_tx_count(tx_count);
	while (offset < buffer_size) {
		TxData tx_data;
		if (msg.is_sid_tx(offset)) {
			uint64_t short_id = 0;
			offset = msg.get_tx_sid(offset, short_id);
			_try_get_tx_data(short_id, tx_data);
			if (tx_data.psha == nullptr) {
				_unknown_tx_sids.push_back(short_id);
			} else if (tx_data.ptx == nullptr) {
				_unknown_tx_hashes.push_back(tx_data.psha);
			}
		} else {
			size_t from = offset;
			offset = msg.get_next_tx_offset(offset);
			tx_data.ptx = nullptr; // TODO : fixme
		}
		if (_unknown_tx_sids.size() == 0 &&
				_unknown_tx_hashes.size() == 0) {
			_output_buffer += *tx_data.ptx;
		} else {
			_partial_block = true;
		}
	}
}

void BTCBlockDecompressionTask::_init_sub_tasks(
		size_t pool_size,
		size_t tx_count
)
{

}

void BTCBlockDecompressionTask::_dispatch(
		size_t tx_count,
		utils::protocols::BTCBlockMessage& msg,
		size_t offset,
		SubPool_t& sub_pool
)
{

}

void BTCBlockDecompressionTask::_enqueue_task(
		size_t task_idx,
		SubPool_t& sub_pool
)
{

}

int BTCBlockDecompressionTask::_try_get_tx_data(
		unsigned int short_id,
		TxData& tx_data
)
{
	int ret = -1;
	auto sha_iter = _short_id_map.find(short_id);
	if (sha_iter != _short_id_map.end()) {
		ret = 0;
		const utils::crypto::Sha256& sha = sha_iter->second;
		tx_data.psha = &sha;
		auto tx_iter = _tx_map.find(sha);
		if (tx_iter != _tx_map.end()) {
			ret = 1;
			tx_data.ptx = nullptr; // TODO : fixme
		}
	}
	return ret;
}

} // task
