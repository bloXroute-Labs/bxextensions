#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <deque>
#include <utility>
#include <list>
#include <thread>

#include <utils/common/buffer_view.h>
#include <utils/concurrency/memory_allocation_thread.h>
#include <utils/concurrency/concurrent_allocator.h>

#include "tpe/consts.h"
#include "tpe/service/transaction_to_short_ids_map.h"

#ifndef TPE_SERVICE_TRANSACTION_SERVICE_H_
#define TPE_SERVICE_TRANSACTION_SERVICE_H_

#define DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT 6

namespace service {

typedef utils::common::BufferView TxContents_t;
typedef std::shared_ptr<TxContents_t> PTxContents_t;
typedef std::shared_ptr<Sha256_t> PSha256_t;
typedef utils::concurrency::ConcurrentAllocator<std::pair<const unsigned int, PSha256_t>> ShortIDToShaAllocator_t;
typedef utils::crypto::Sha256Allocator_t Sha256Allocator_t;
typedef utils::crypto::Sha256MapAllocator_t<PTxContents_t> Sha256ContentMapAllocator_t;
typedef std::unordered_map<unsigned int, PSha256_t, std::hash<unsigned int>, std::equal_to<unsigned int>, ShortIDToShaAllocator_t> ShortIDToSha256Map_t;
typedef utils::crypto::Sha256Map_t<PTxContents_t> Sha256ToContentMap_t;
typedef std::vector<PSha256_t> UnknownTxHashes_t;
typedef std::vector<unsigned int> ShortIDs_t;
typedef std::pair<size_t, ShortIDs_t> TrackSeenResult_t;
typedef utils::concurrency::MemoryAllocationThread MemoryAllocationThread_t;


struct Allocators {
    Allocators(
            size_t max_allocation_pointer_count, size_t max_allocations_by_size, MemoryAllocationThread_t* memory_thread
    ):
            sha256_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread),
            short_id_to_sha_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread),
            sha256_to_content_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread),
            sha256_to_short_ids_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread)
    {

    }
    ShortIDToShaAllocator_t short_id_to_sha_allocator;
    Sha256Allocator_t sha256_allocator;
    Sha256ContentMapAllocator_t sha256_to_content_allocator;
    Sha256ToShortIDsAllocator_t sha256_to_short_ids_allocator;

};

struct Containers {

    Containers(
            size_t pool_size,
            size_t tx_bucket_capacity,
            Allocators& allocators
    ):
        tx_not_seen_in_blocks(tx_bucket_capacity, pool_size, allocators.sha256_allocator),
        tx_hash_to_contents(allocators.sha256_to_content_allocator),
        short_id_to_tx_hash(allocators.short_id_to_sha_allocator),
        tx_hash_to_short_ids(tx_not_seen_in_blocks, allocators.sha256_to_short_ids_allocator)
    {

    }

    TxNotSeenInBlocks_t tx_not_seen_in_blocks;
    Sha256ToShortIdsMap tx_hash_to_short_ids;
    ShortIDToSha256Map_t short_id_to_tx_hash;
    Sha256ToContentMap_t tx_hash_to_contents;
};

class TransactionService {
public:

	TransactionService(
	        size_t pool_size,
	        size_t tx_bucket_capacity = BTC_DEFAULT_TX_BUCKET_SIZE,
	        size_t final_tx_confirmations_count = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT,
	        size_t max_allocation_pointer_count = MAX_ALLOCATION_POINTER_COUNT,
	        size_t max_count_per_allocation = MAX_COUNT_PER_ALLOCATION,
            int64_t thread_loop_sleep_microseconds = ALLOCATION_LOOP_SLEEP_MICROSECONDS
    );

	Sha256ToShortIDsMap_t& get_tx_hash_to_short_ids();
	ShortIDToSha256Map_t& get_short_id_to_tx_hash();
	Sha256ToContentMap_t& get_tx_hash_to_contents();
    TxNotSeenInBlocks_t& tx_not_seen_in_blocks();

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

	std::deque<ShortIDs_t> _short_ids_seen_in_block;
    size_t _final_tx_confirmations_count;
    MemoryAllocationThread_t _allocation_thread;
    Allocators _allocators;
    Containers _containers;
};


} // service


#endif /* TPE_SERVICE_TRANSACTION_SERVICE_H_ */
