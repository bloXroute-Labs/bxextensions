#include "tpe/service/compact_block_data_service.h"

namespace service {

CompactBlockDataService::CompactBlockDataService():
		_msg(nullptr),
		_short_id_map(nullptr),
		_total_tx_count(0),
		_compact_tx_count(0)
{
}

CompactBlockDataService::CompactBlockDataService(
		PBufferView_t block_buffer,
		const Sha256ToShortIDsMap_t* short_id_map
):
		_short_id_map(short_id_map),
		_block_buffer(block_buffer),
		_total_tx_count(0),
		_compact_tx_count(0)
{
	_msg = std::make_shared<BtcCompactBlockMessage_t>(*block_buffer);
}

CompactBlockDataService::CompactBlockDataService(
		CompactBlockDataService&& rhs
):
		_msg(std::move(rhs._msg)),
		_short_id_map(rhs._short_id_map),
		_total_tx_count(rhs._total_tx_count),
		_compact_tx_count(rhs._compact_tx_count)
{
	_tx_placeholders = std::move(rhs._tx_placeholders);
	_block_buffer = std::move(rhs._block_buffer);

}

CompactBlockDataService&
CompactBlockDataService::operator =(CompactBlockDataService&& rhs) {
	_short_id_map = rhs._short_id_map;
	_msg = std::move(rhs._msg);
	_block_buffer = std::move(rhs._block_buffer);
	_tx_placeholders = std::move(rhs._tx_placeholders);
	_total_tx_count = rhs._total_tx_count;
	return *this;
}

void CompactBlockDataService::parse() {
	_sip_key = std::move(_msg->get_block_sip_key());
	uint64_t pre_filled_tx_count;
	size_t compact_offset =
			_msg->get_compact_txs_count(_compact_tx_count);
	size_t offset =
			compact_offset + (_compact_tx_count * CompactShortId_t::size);
	offset =
			_msg->get_pre_filled_txs_count(offset, pre_filled_tx_count);
	_total_tx_count = _compact_tx_count + pre_filled_tx_count;
	_tx_placeholders.reserve(_total_tx_count);
	_compact_map.reserve(_compact_tx_count);
	uint64_t idx = 0;
	uint64_t compact_count = 0;
	for( ; idx < pre_filled_tx_count ; ++idx) {
		uint64_t diff;
		Sha256_t prefilled_sha;
		TransactionPlaceholder prefilled_placeholder;
		prefilled_placeholder.contents = std::move(
				_msg->get_next_pre_filled_tx(
					offset,
					diff,
					prefilled_sha
				)
		);
		auto iter = _short_id_map->find(prefilled_sha);
		if (iter == _short_id_map->end()) {
			prefilled_placeholder.type = TransactionPlaceholderType::content;
		} else {
			prefilled_placeholder.type = TransactionPlaceholderType::short_id;
			prefilled_placeholder.sid = *iter->second.begin();
		}

		uint64_t compact_diff = diff / CompactShortId_t::size;
		compact_count += _fill_compact_ids(
				compact_diff, compact_offset, idx + compact_count
		);
		_tx_placeholders.push_back(prefilled_placeholder);
	}
	uint64_t compact_diff = _compact_tx_count - compact_count;
	_fill_compact_ids(compact_diff, compact_offset, idx + compact_count);
}

const SipKey_t& CompactBlockDataService::sip_key() const {
	return _sip_key;
}

const BtcCompactBlockMessage_t& CompactBlockDataService::msg() const {
	return *_msg;
}

size_t CompactBlockDataService::size(void) const {
	return _block_buffer->size();
}

uint64_t CompactBlockDataService::total_tx_count() const {
	return _total_tx_count;
}

uint64_t CompactBlockDataService::compact_tx_count() const {
	return _compact_tx_count;
}

CompactTransactionPlaceholders_t&
CompactBlockDataService::tx_placeholders() {
	return _tx_placeholders;
}

CompactShortIdToShortIdMap_t&
CompactBlockDataService::compact_map() {
	return _compact_map;
}

void CompactBlockDataService::on_compression_completed() {
	_block_buffer = nullptr;
	_msg = nullptr;
	_short_id_map = nullptr;
}

uint64_t CompactBlockDataService::_fill_compact_ids(
		uint64_t compact_diff,
		size_t& offset,
		uint64_t idx
)
{
	uint64_t compact_idx = 0;
	for(
			;
			compact_idx < compact_diff ;
			++compact_idx
	)
	{
		TransactionPlaceholder placeholder;
		placeholder.cmpt = std::move(
				_msg->get_next_compact_short_id(offset)
		);
		_tx_placeholders.push_back(
				placeholder
		);
		_compact_map.insert(std::make_pair(
				*placeholder.cmpt,
				std::make_pair(_msg->null_tx_sid, idx + compact_idx)
		));
	}
	return compact_idx;
}



} // service
