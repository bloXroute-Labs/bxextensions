#include "tpe/service/transaction_service.h"

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

size_t TransactionService::size() const {
	return _tx_hash_to_short_ids.size();
}

} // service
