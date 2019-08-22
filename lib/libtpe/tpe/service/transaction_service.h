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
typedef utils::crypto::Sha256MapWrapper_t<PTxContents_t> Sha256ToContentMap_t;
typedef std::vector<PSha256_t> UnknownTxHashes_t;
typedef std::vector<unsigned int> ShortIDs_t;
typedef std::pair<size_t, ShortIDs_t> TrackSeenResult_t;
typedef utils::concurrency::MemoryAllocationThread MemoryAllocationThread_t;
typedef utils::common::AbstractValueTracker<PTxContents_t> AbstractValueTracker_t;

struct PTxContentsAllocator: public AbstractValueTracker_t {

    explicit PTxContentsAllocator(MemoryAllocationThread_t& memory_thread):
        _memory_thread(memory_thread),
        _total_bytes_allocated(0)
    {
        _idx = _register();
    }

    PTxContentsAllocator(const PTxContentsAllocator& other):
        _memory_thread(other._memory_thread),
        _total_bytes_allocated(other._total_bytes_allocated)
    {
        _idx = _register();
    }

    ~PTxContentsAllocator() override {
        { // lock scope
            std::lock_guard<std::mutex> lock(_mtx);
            _memory_thread.remove(_idx);
        }
        _deallocation_queue.clear();
    }

    void on_value_removed(PTxContents_t&& p_contents) override {
        { // lock scope
            std::lock_guard<std::mutex> lock(_mtx);
            _total_bytes_allocated -= (p_contents->size() + sizeof(PTxContents_t));
            _deallocation_queue.emplace_back(std::move(p_contents));
        }
        _memory_thread.notify(_idx);
    }

    void on_value_added(const PTxContents_t& p_contents) override {
        std::lock_guard<std::mutex> lock(_mtx);
        _total_bytes_allocated += (p_contents->size() + sizeof(PTxContents_t));
    }

    size_t total_bytes_allocated() const override {
        return _total_bytes_allocated;
    }

private:

    uint32_t _register() {
        return _memory_thread.add([&](){
            std::list<PTxContents_t> deallocation_queue_copy;
            { // lock scope
                std::lock_guard<std::mutex> lock(_mtx);
                deallocation_queue_copy.swap(_deallocation_queue);
            }
            deallocation_queue_copy.clear();
        });
    }

    MemoryAllocationThread_t& _memory_thread;
    std::mutex _mtx;
    std::list<PTxContents_t> _deallocation_queue;
    uint32_t _idx;
    volatile size_t _total_bytes_allocated;
};

struct Allocators {
    Allocators(
            size_t max_allocation_pointer_count, size_t max_allocations_by_size, MemoryAllocationThread_t* memory_thread
    ):
            p_tx_contents_allocator(*memory_thread),
            sha256_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread),
            short_id_to_sha_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread),
            sha256_to_content_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread),
            sha256_to_short_ids_allocator(max_allocation_pointer_count, max_allocations_by_size, memory_thread)
    {

    }
    PTxContentsAllocator p_tx_contents_allocator;
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
        tx_hash_to_contents(allocators.p_tx_contents_allocator, allocators.sha256_to_content_allocator),
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
	        size_t max_count_per_allocation = MAX_COUNT_PER_ALLOCATION
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
