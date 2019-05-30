#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <utils/crypto/sha256.h>
#include <utils/common/buffer_view.h>

#ifndef TPE_SERVICE_TRANSACTION_SERVICE_H_
#define TPE_SERVICE_TRANSACTION_SERVICE_H_


namespace service {

template <typename T>
using Sha256DefaultMap_t = utils::crypto::Sha256DefaultMap_t<T>;
typedef std::shared_ptr<utils::common::BufferView> TxContents_t;
typedef utils::crypto::Sha256 Sha256_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;
typedef Sha256DefaultMap_t<std::unordered_set<unsigned int>> Sha256ToShortIDsMap_t;
typedef std::unordered_map<unsigned int, PSha256_t> ShortIDToSha256Map_t;
typedef utils::crypto::Sha256Map_t<TxContents_t> Sha256ToContentMap_t;
typedef std::vector<PSha256_t> UnknownTxHashes_t;
typedef std::vector<unsigned int> UnknownTxSIDs_t;

class TransactionService {
public:
	Sha256ToShortIDsMap_t& tx_hash_to_short_ids();
	ShortIDToSha256Map_t& short_id_to_tx_hash();
	Sha256ToContentMap_t& tx_hash_to_contents();

	bool has_short_id(const Sha256_t& tx_hash) const;

	unsigned int get_short_id(const Sha256_t& tx_hash) const;

	TxContents_t get_transaction(
			unsigned int short_id, Sha256_t& tx_hash
	);

	size_t size(void) const;
	size_t get_tx_size(unsigned int short_id) const;
	bool get_missing_transactions(
			UnknownTxHashes_t& unknown_tx_hashes,
			UnknownTxSIDs_t& unknown_tx_sids,
			const std::vector<unsigned int>& short_ids
	) const;

private:
	Sha256ToShortIDsMap_t _tx_hash_to_short_ids;
	ShortIDToSha256Map_t _short_id_to_tx_hash;
	Sha256ToContentMap_t _tx_hash_to_contents;
};


} // service


#endif /* TPE_SERVICE_TRANSACTION_SERVICE_H_ */