#include "tpe/service/transaction_service.h"


namespace service {

TransactionService::TransactionService(
        size_t pool_size,
        size_t tx_bucket_capacity/* = BTC_DEFAULT_TX_BUCKET_SIZE*/,
        size_t final_tx_confirmations_count/* = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT*/
):
		_tx_not_seen_in_blocks(tx_bucket_capacity, pool_size),
		_tx_hash_to_short_ids(_tx_not_seen_in_blocks),
		_final_tx_confirmations_count(final_tx_confirmations_count)
{

}

Sha256ToShortIDsMap_t& TransactionService::get_tx_hash_to_short_ids() {
	return _tx_hash_to_short_ids;
}

ShortIDToSha256Map_t& TransactionService::get_short_id_to_tx_hash() {
	return _short_id_to_tx_hash;
}

Sha256ToContentMap_t& TransactionService::get_tx_hash_to_contents() {
	return _tx_hash_to_contents;
}

const Sha256ToShortIDsMap_t&
TransactionService::tx_hash_to_short_ids() const {
	return _tx_hash_to_short_ids;
}

const Sha256ToContentMap_t&
TransactionService::tx_hash_to_contents() const {
	return _tx_hash_to_contents;
}

bool TransactionService::has_short_id(const Sha256_t& tx_hash) const {
	auto iter = _tx_hash_to_short_ids.find(tx_hash);
	return iter != _tx_hash_to_short_ids.end();
}

unsigned int TransactionService::get_short_id(
		const Sha256_t& tx_hash
) const {
	auto iter = _tx_hash_to_short_ids.find(tx_hash);
	return *iter->second.begin();
}

PTxContents_t TransactionService::get_transaction(
		unsigned int short_id, Sha256_t& tx_hash
)
{
	tx_hash = *_short_id_to_tx_hash[short_id];
	return _tx_hash_to_contents[tx_hash];
}

const TxContents_t* TransactionService::get_tx_contents_raw_ptr(
        unsigned int short_id
)
{
    const Sha256_t& tx_hash = *_short_id_to_tx_hash[short_id];
    return _tx_hash_to_contents[tx_hash].get();
}

size_t TransactionService::size() const {
	return _tx_hash_to_short_ids.size();
}

size_t TransactionService::get_tx_size(unsigned int short_id) const {
	Sha256_t& tx_hash = *_short_id_to_tx_hash.at(short_id);
	return _tx_hash_to_contents.at(tx_hash)->size();
}

bool TransactionService::get_missing_transactions(
		UnknownTxHashes_t& unknown_tx_hashes,
		ShortIDs_t& unknown_tx_sids,
		const std::vector<unsigned int>& short_ids
) const
{
	bool success = true;
	for (unsigned int short_id: short_ids) {
		auto sid_iter = _short_id_to_tx_hash.find(short_id);
		if (sid_iter != _short_id_to_tx_hash.end() && short_id != NULL_TX_SID) {
			auto hash_iter = _tx_hash_to_contents.find(*sid_iter->second);
			if (hash_iter == _tx_hash_to_contents.end()) {
				unknown_tx_hashes.push_back(sid_iter->second);
				success = false;
			}
		} else {
			unknown_tx_sids.push_back(short_id);
			success = false;
		}
	}
	return success;
}

const TxNotSeenInBlocks_t& TransactionService::acquire_tx_pool() {
	_tx_not_seen_in_blocks.acquire_read();
	return _tx_not_seen_in_blocks;
}

void TransactionService::on_finished_reading_tx_pool() {
	_tx_not_seen_in_blocks.release_read();
}

void TransactionService::track_seen_transaction(const Sha256_t &sha) {
	_tx_not_seen_in_blocks.erase(sha);
}

// TODO : add a lock to this function to prevent a race condition between data structures
TrackSeenResult_t TransactionService::track_seen_short_ids(
        const ShortIDs_t &short_ids
)
{
    TrackSeenResult_t result;
    result.first = 0;
    _short_ids_seen_in_block.push_back(short_ids);
    for (const unsigned int& short_id: short_ids) {
        auto sha_itr = _short_id_to_tx_hash.find(short_id);
        if (sha_itr != _short_id_to_tx_hash.end()) {
            _tx_not_seen_in_blocks.erase(*sha_itr->second);
        }
    }
    if (_short_ids_seen_in_block.size() >= _final_tx_confirmations_count) {
        auto& final_short_ids = _short_ids_seen_in_block.front();
        for (const unsigned int& short_id: final_short_ids) {
            result.first +=
                    _remove_transaction_by_short_id(short_id, result.second);
        }
        _short_ids_seen_in_block.pop_front();
    }
    return std::move(result);
}

void TransactionService::set_final_tx_confirmations_count(size_t val) {
    _final_tx_confirmations_count = val;
}

size_t TransactionService::_remove_transaction_by_short_id(
        unsigned int short_id, ShortIDs_t& dup_sids
)
{
    auto sha_iter = _short_id_to_tx_hash.find(short_id);
    size_t contents_len = 0;
    if (sha_iter != _short_id_to_tx_hash.end()) {
        const Sha256_t& sha = *sha_iter->second;
        auto& short_ids = _tx_hash_to_short_ids.at(sha);
        if (short_ids.size() > 1) {
            short_ids.erase(short_id);
            for (const unsigned int& dup_short_id : short_ids) {
                _short_id_to_tx_hash.erase(dup_short_id);
                dup_sids.push_back(dup_short_id);
            }
        }
        _tx_hash_to_short_ids.erase(sha);
        auto tx_hash_iter = _tx_hash_to_contents.find(sha);
        if (tx_hash_iter != _tx_hash_to_contents.end()) {
            contents_len = (tx_hash_iter->second)->size();
            _tx_hash_to_contents.erase(tx_hash_iter);
        }
        // TODO: if tx_hash_iter == _tx_hash_to_contents.end(): write error to the log
        _short_id_to_tx_hash.erase(sha_iter);
    }
    return contents_len;
}

} // service
