#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <deque>
#include <utility>
#include <list>
#include <thread>

#include <utils/common/buffer_view.h>
#include <utils/common/ordered_map.h>
#include <utils/common/tracked_allocator.h>
#include <utils/common/byte_array.h>

#include "tpe/consts.h"
#include "tpe/service/transaction_to_short_ids_map.h"

#ifndef TPE_SERVICE_TRANSACTION_SERVICE_H_
#define TPE_SERVICE_TRANSACTION_SERVICE_H_

#define DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT 6

namespace service {

typedef utils::common::BufferView TxContents_t;
typedef std::shared_ptr<TxContents_t> PTxContents_t;
typedef utils::common::ByteArray TxSyncTxs_t;
typedef std::shared_ptr<TxSyncTxs_t> PTxSyncTxs_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;
typedef std::vector<unsigned int> ShortIDs_t;
typedef utils::common::TrackedAllocator<std::pair<const unsigned int, PSha256_t>> ShortIDToShaAllocator_t;
typedef std::unordered_map<unsigned int, PSha256_t, std::hash<unsigned int>, std::equal_to<unsigned int>, ShortIDToShaAllocator_t> ShortIDToSha256Map_t;
typedef utils::crypto::Sha256MapWrapper_t<PTxContents_t> Sha256ToContentMap_t;
typedef std::vector<PSha256_t> UnknownTxHashes_t;
typedef std::pair<size_t, ShortIDs_t> TrackSeenResult_t;
typedef utils::crypto::Sha256OrderedMap_t<ShortIDs_t> ShortIDsSeenInBlock_t;
typedef utils::common::AbstractValueTracker<PTxContents_t> AbstractValueTracker_t;
typedef utils::crypto::Sha256OrderedMap_t<double> Sha256ToTime_t;



struct PTxContentsTracker: public AbstractValueTracker_t {

    PTxContentsTracker():
        _total_bytes_allocated(0)
    {
    }

    PTxContentsTracker(const PTxContentsTracker& other):
        _total_bytes_allocated(other._total_bytes_allocated)
    {
    }

    void on_value_removed(PTxContents_t&& p_contents) override {
        _total_bytes_allocated -= p_contents->size();
    }

    void on_value_added(const PTxContents_t& p_contents) override {
        _total_bytes_allocated += p_contents->size();
    }

    size_t total_bytes_allocated() const override {
        return _total_bytes_allocated;
    }

    void on_container_cleared() override {
        _total_bytes_allocated = 0;
    }

private:

    volatile size_t _total_bytes_allocated;
};

struct Containers {

    Containers(
            size_t pool_size,
            size_t tx_bucket_capacity
    ):
        tx_not_seen_in_blocks(tx_bucket_capacity, pool_size),
        tx_hash_to_contents(PTxContentsTracker()),
        short_id_to_tx_hash(),
        tx_hash_to_short_ids(tx_not_seen_in_blocks),
        short_ids_seen_in_block(),
        tx_hash_to_time_removed()

    {

    }

    TxNotSeenInBlocks_t tx_not_seen_in_blocks;
    Sha256ToShortIdsMap tx_hash_to_short_ids;
    ShortIDToSha256Map_t short_id_to_tx_hash;
    Sha256ToContentMap_t tx_hash_to_contents;
    ShortIDsSeenInBlock_t short_ids_seen_in_block;
    Sha256ToTime_t tx_hash_to_time_removed;

};

class TransactionService {
public:

	TransactionService(
	        size_t pool_size,
	        size_t tx_bucket_capacity = BTC_DEFAULT_TX_BUCKET_SIZE,
	        size_t final_tx_confirmations_count = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT
    );

	Sha256ToShortIDsMap_t& get_tx_hash_to_short_ids();
	ShortIDToSha256Map_t& get_short_id_to_tx_hash();
	PTxSyncTxs_t get_tx_sync_buffer(size_t all_txs_content_size, bool sync_tx_content);
	Sha256ToContentMap_t& get_tx_hash_to_contents();
    TxNotSeenInBlocks_t& tx_not_seen_in_blocks();
    Sha256ToTime_t& tx_hash_to_time_removed();

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

    TrackSeenResult_t track_seen_short_ids(const Sha256_t& block_hash, ShortIDs_t short_ids);

    void on_finished_reading_tx_pool();
    void track_seen_transaction(const Sha256_t &sha);
    void on_block_cleaned_up(const Sha256_t& block_hash);
    void set_final_tx_confirmations_count(size_t val);
    size_t remove_transactions_by_hashes(const std::vector<Sha256_t>& tx_hashes);
    size_t remove_transaction_by_hash(const Sha256_t& sha);
    size_t remove_transaction_by_short_id(
            unsigned int short_id, ShortIDs_t &dup_sids
    );

    void clear_short_ids_seen_in_block();

private:

    size_t _final_tx_confirmations_count;
    Containers _containers;
};


} // service


#endif /* TPE_SERVICE_TRANSACTION_SERVICE_H_ */
