#include "tpe/service/transaction_service.h"
#include "tpe/consts.h"

namespace service {

Sha256ToShortIDsMap_t& TransactionService::tx_hash_to_short_ids() {
	return _tx_hash_to_short_ids;
}

ShortIDToSha256Map_t& TransactionService::short_id_to_tx_hash() {
	return _short_id_to_tx_hash;
}

Sha256ToContentMap_t& TransactionService::tx_hash_to_contents() {
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

TxContents_t TransactionService::get_transaction(
		unsigned int short_id, Sha256_t& tx_hash
)
{
	tx_hash = *_short_id_to_tx_hash[short_id];
	return _tx_hash_to_contents[tx_hash];
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
		UnknownTxSIDs_t& unknown_tx_sids,
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


} // service
