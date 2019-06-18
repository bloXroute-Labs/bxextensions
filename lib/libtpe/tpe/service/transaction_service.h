#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <utils/common/buffer_view.h>

#include "tpe/service/transaction_to_short_ids_map.h"

#ifndef TPE_SERVICE_TRANSACTION_SERVICE_H_
#define TPE_SERVICE_TRANSACTION_SERVICE_H_


namespace service {

typedef std::shared_ptr<utils::common::BufferView> TxContents_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;
typedef std::unordered_map<unsigned int, PSha256_t> ShortIDToSha256Map_t;
typedef utils::crypto::Sha256Map_t<TxContents_t> Sha256ToContentMap_t;
typedef std::vector<PSha256_t> UnknownTxHashes_t;
typedef std::vector<unsigned int> UnknownTxSIDs_t;

class TransactionService {
public:

	TransactionService();

	Sha256ToShortIDsMap_t& get_tx_hash_to_short_ids();
	ShortIDToSha256Map_t& get_short_id_to_tx_hash();
	Sha256ToContentMap_t& get_tx_hash_to_contents();

	const Sha256ToShortIDsMap_t& tx_hash_to_short_ids() const;
	const Sha256ToContentMap_t& tx_hash_to_contents() const;

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

	const TxNotSeenInBlocks_t& acquire_tx_pool(void);

	void on_finished_reading_tx_pool(void);

	void remove_from_tx_pool(const Sha256_t& sha);

private:
	Sha256ToShortIdsMap _tx_hash_to_short_ids;
	ShortIDToSha256Map_t _short_id_to_tx_hash;
	Sha256ToContentMap_t _tx_hash_to_contents;
	TxNotSeenInBlocks_t _tx_not_seen_in_blocks;
};


} // service


#endif /* TPE_SERVICE_TRANSACTION_SERVICE_H_ */
