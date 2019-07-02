#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <deque>
#include <utility>

#include <utils/common/buffer_view.h>

#include "tpe/consts.h"
#include "tpe/service/transaction_to_short_ids_map.h"

#ifndef TPE_SERVICE_TRANSACTION_SERVICE_H_
#define TPE_SERVICE_TRANSACTION_SERVICE_H_

#define DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT 6

namespace service {

typedef utils::common::BufferView TxContents_t;
typedef std::shared_ptr<TxContents_t> PTxContents_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;
typedef std::unordered_map<unsigned int, PSha256_t> ShortIDToSha256Map_t;
typedef utils::crypto::Sha256Map_t<PTxContents_t> Sha256ToContentMap_t;
typedef std::vector<PSha256_t> UnknownTxHashes_t;
typedef std::vector<unsigned int> ShortIDs_t;
typedef std::pair<size_t, ShortIDs_t> TrackSeenResult_t;

class TransactionService {
public:

	TransactionService(
	        size_t pool_size,
	        size_t tx_bucket_capacity = BTC_DEFAULT_TX_BUCKET_SIZE,
	        size_t final_tx_confirmations_count = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT
    );

	Sha256ToShortIDsMap_t& get_tx_hash_to_short_ids();
	ShortIDToSha256Map_t& get_short_id_to_tx_hash();
	Sha256ToContentMap_t& get_tx_hash_to_contents();

	const Sha256ToShortIDsMap_t& tx_hash_to_short_ids() const;
	const Sha256ToContentMap_t& tx_hash_to_contents() const;

	bool has_short_id(const Sha256_t& tx_hash) const;

	unsigned int get_short_id(const Sha256_t& tx_hash) const;

	// TODO : use this instead of the hack below!
	PTxContents_t get_transaction(
			unsigned int short_id, Sha256_t& tx_hash
	);

	// TODO : remove this hack!
	const TxContents_t* get_tx_contents_raw_ptr(unsigned int short_id);

	size_t size() const;
	size_t get_tx_size(unsigned int short_id) const;
	bool get_missing_transactions(
			UnknownTxHashes_t& unknown_tx_hashes,
			ShortIDs_t& unknown_tx_sids,
			const std::vector<unsigned int>& short_ids
	) const;

	const TxNotSeenInBlocks_t& acquire_tx_pool();

	void on_finished_reading_tx_pool();

	void track_seen_transaction(const Sha256_t &sha);
    TrackSeenResult_t track_seen_short_ids(const ShortIDs_t &short_ids);
    void set_final_tx_confirmations_count(size_t val);

private:

    size_t _remove_transaction_by_short_id(
            unsigned int short_id, ShortIDs_t& dup_sids
    );

	Sha256ToShortIdsMap _tx_hash_to_short_ids;
	ShortIDToSha256Map_t _short_id_to_tx_hash;
	Sha256ToContentMap_t _tx_hash_to_contents;
	TxNotSeenInBlocks_t _tx_not_seen_in_blocks;
	std::deque<ShortIDs_t> _short_ids_seen_in_block;
	size_t _final_tx_confirmations_count;
};


} // service


#endif /* TPE_SERVICE_TRANSACTION_SERVICE_H_ */
