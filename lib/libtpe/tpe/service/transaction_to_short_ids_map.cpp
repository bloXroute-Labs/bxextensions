#include "transaction_to_short_ids_map.h"

namespace service {

Sha256ToShortIdsMap::Sha256ToShortIdsMap(
		TxNotSeenInBlocks_t& tx_not_seen
): Sha256ToShortIDsMap_t(), _tx_not_seen(tx_not_seen)
{
}

TxShortIds_t& Sha256ToShortIdsMap::operator[](
		const Sha256_t& key
)
{
	auto iter = _map.find(key);
	if(iter == end()) {
		_map[key] = std::move(_item_factory());
		_tx_not_seen.insert(key);
	}
	return _map[key];
}

void Sha256ToShortIdsMap::erase(const Sha256_t& key) {
	_map.erase(key);
	_tx_not_seen.erase(key);
}

void Sha256ToShortIdsMap::clear() {
	_map.clear();
	_tx_not_seen.clear();
}

} // service


