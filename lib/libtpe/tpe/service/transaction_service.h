#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <deque>
#include <utility>
#include <list>
#include <thread>

#include <utils/common/buffer_view.h>
#include <utils/common/buffer_copy.h>
#include <utils/common/byte_array.h>
#include <utils/common/ordered_map.h>
#include <utils/common/tracked_allocator.h>
#include <utils/common/byte_array.h>
#include <utils/protocols/abstract_message_parser.h>
#include <utils/protocols/abstract_transaction_validator.h>
#include <utils/common/tx_status_consts.h>
#include <utils/common/tx_validation_status_consts.h>

#include "tpe/consts.h"
#include "tpe/service/transaction_to_short_ids_map.h"

#ifndef TPE_SERVICE_TRANSACTION_SERVICE_H_
#define TPE_SERVICE_TRANSACTION_SERVICE_H_

#define DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT 6
#define NULL_TX_TIMESTAMP 0
#define MAX_TRANSACTION_ELAPSED_TIME_S 10

#define has_status_flag(tx_status, flag) (tx_status & flag)

namespace service {

typedef utils::common::BufferCopy BufferCopy_t;
typedef utils::common::ByteArray ByteArray_t;
typedef utils::common::BufferView TxContents_t;
typedef std::shared_ptr<TxContents_t> PTxContents_t;
typedef utils::common::BufferView TxsMsg_t;
typedef std::shared_ptr<TxsMsg_t> PTxsMsg_t;
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
typedef utils::common::OrderedMap<unsigned int, double> ShortIdToTime_t;
typedef utils::common::AbstractValueTracker<PTxContents_t> AbstractValueTracker_t;
typedef utils::crypto::Sha256OrderedMap_t<double> Sha256ToTime_t;
typedef bool AssignShortIDResult_t;
typedef std::pair<bool, unsigned int> SetTransactionContentsResult_t;
typedef utils::common::BufferView TxsMessageContents_t;
typedef std::shared_ptr<TxsMessageContents_t> PTxsMessageContents_t;
typedef utils::common::BufferView SearializedShortIds_t;
typedef utils::protocols::AbstractMessageParser AbstractMessageParser_t;
typedef utils::protocols::AbstractTransactionValidator AbstractTransactionValidator_t;
typedef unsigned int TxStatus_t;
typedef unsigned int TxValidationStatus_t;

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
        tx_hash_to_short_ids(tx_not_seen_in_blocks),
        short_id_to_tx_hash(),
        tx_hash_to_contents(PTxContentsTracker()),
        short_ids_seen_in_block(),
        short_id_to_assign_time(),
        tx_hash_to_time_removed(),
        short_id_to_time_removed()
    {
    }

    TxNotSeenInBlocks_t tx_not_seen_in_blocks;
    Sha256ToShortIdsMap tx_hash_to_short_ids;
    ShortIDToSha256Map_t short_id_to_tx_hash;
    Sha256ToContentMap_t tx_hash_to_contents;
    ShortIDsSeenInBlock_t short_ids_seen_in_block;
    ShortIdToTime_t short_id_to_assign_time;
    Sha256ToTime_t tx_hash_to_time_removed;
    ShortIdToTime_t short_id_to_time_removed;
};

class TxProcessingResult {
public:
    TxProcessingResult(
        unsigned int tx_status,
        unsigned int tx_validation_status,
        TxShortIds_t existing_short_ids,
        AssignShortIDResult_t assign_short_id_result,
        SetTransactionContentsResult_t set_transaction_contents_result,
        bool contents_set,
        bool short_id_assigned
        ) : _tx_status(tx_status),
            _tx_validation_status(tx_validation_status),
            _existing_short_ids(existing_short_ids),
            _assign_short_id_result(assign_short_id_result),
            _set_transaction_contents_result(set_transaction_contents_result),
            _contents_set(contents_set),
            _short_id_assigned(short_id_assigned) {
    }

    TxProcessingResult(
        unsigned int tx_status,
        unsigned int tx_validation_status,
        SetTransactionContentsResult_t set_transaction_contents_result,
        bool contents_set
        ) : _tx_status(tx_status),
            _tx_validation_status(tx_validation_status),
            _assign_short_id_result(false),
            _set_transaction_contents_result(set_transaction_contents_result),
            _contents_set(contents_set),
            _short_id_assigned()
    {
    }

    TxProcessingResult(
        unsigned int tx_status,
        unsigned int tx_validation_status
        ) : _tx_status(tx_status),
            _tx_validation_status(tx_validation_status),
            _assign_short_id_result(false),
            _set_transaction_contents_result(),
            _contents_set(),
            _short_id_assigned()
    {
    }

    unsigned int get_tx_status() {
        return _tx_status;
    }

    unsigned int get_tx_validation_status() {
        return _tx_validation_status;
    }

    TxShortIds_t get_existing_short_ids() {
        return _existing_short_ids;
    }

    bool get_short_id_assigned() {
        return _short_id_assigned;
    }

    AssignShortIDResult_t get_assign_short_id_result() {
        return _assign_short_id_result;
    }

    bool get_contents_set() {
        return _contents_set;
    }

    SetTransactionContentsResult_t get_set_transaction_contents_result() {
        return _set_transaction_contents_result;
    }

private:
    unsigned int _tx_status;
    unsigned int _tx_validation_status;
    TxShortIds_t _existing_short_ids;
    AssignShortIDResult_t _assign_short_id_result;
    SetTransactionContentsResult_t _set_transaction_contents_result;
    bool _contents_set;
    bool _short_id_assigned;
};

typedef TxProcessingResult TxProcessingResult_t;
typedef std::shared_ptr<TxProcessingResult_t> PTxProcessingResult_t;

class TxFromBdnProcessingResult {

public:
    TxFromBdnProcessingResult(
        bool ignore_seen,
        bool existing_short_id,
        bool assigned_short_id,
        bool existing_contents,
        bool set_contents,
        SetTransactionContentsResult_t set_contents_result
    ) : _ignore_seen(ignore_seen),
        _existing_short_id(existing_short_id),
        _assigned_short_id(assigned_short_id),
        _existing_contents(existing_contents),
        _set_contents(set_contents),
        _set_contents_result(set_contents_result) {}

    TxFromBdnProcessingResult(
        bool ignore_seen
    ) : _ignore_seen(ignore_seen),
        _existing_short_id(),
        _assigned_short_id(),
        _existing_contents(),
        _set_contents(),
        _set_contents_result() {}

    bool get_ignore_seen() {
        return _ignore_seen;
    }

    bool get_existing_short_id() {
        return _existing_short_id;
    }

    bool get_assigned_short_id() {
        return _assigned_short_id;
    }

    bool get_existing_contents() {
        return _existing_contents;
    }

    bool get_set_contents() {
        return _set_contents;
    }

    SetTransactionContentsResult_t get_set_contents_result() {
        return _set_contents_result;
    }

private:
    bool _ignore_seen;
    bool _existing_short_id;
    bool _assigned_short_id;
    bool _existing_contents;
    bool _set_contents;
    SetTransactionContentsResult_t _set_contents_result;
};

typedef TxFromBdnProcessingResult TxFromBdnProcessingResult_t;
typedef std::shared_ptr<TxFromBdnProcessingResult> PTxFromBdnProcessingResult_t;

typedef utils::common::BufferView ParsedTxContents_t;
typedef std::shared_ptr<ParsedTxContents_t> PParsedTxContents_t;

typedef utils::common::ByteArray ByteArray_t;
typedef std::shared_ptr<ByteArray_t> PByteArray_t;


class TransactionService {
public:

	TransactionService(
        size_t pool_size,
        std::string protocol,
        size_t tx_bucket_capacity = BTC_DEFAULT_TX_BUCKET_SIZE,
        size_t final_tx_confirmations_count = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT
    );

	Sha256ToShortIDsMap_t& get_tx_hash_to_short_ids();
	ShortIDToSha256Map_t& get_short_id_to_tx_hash();
	PTxSyncTxs_t get_tx_sync_buffer(size_t all_txs_content_size, bool sync_tx_content);
	Sha256ToContentMap_t& get_tx_hash_to_contents();
    double get_short_id_assign_time(unsigned int short_id);
    TxNotSeenInBlocks_t& tx_not_seen_in_blocks();
    Sha256ToTime_t& tx_hash_to_time_removed();
    ShortIdToTime_t& short_id_to_time_removed();

	const Sha256ToShortIDsMap_t& tx_hash_to_short_ids() const;
	const Sha256ToContentMap_t& tx_hash_to_contents() const;

	AssignShortIDResult_t assign_short_id(const Sha256_t& transaction_hash, unsigned int short_id);
	SetTransactionContentsResult_t set_transaction_contents(
	        const Sha256_t& transaction_hash,
	        PTxContents_t transaction_contents);

	bool has_short_id(const Sha256_t& tx_hash) const;
	bool has_short_id(unsigned int short_id) const;
	bool has_transaction_contents(const Sha256_t& tx_hash) const;
	bool removed_transaction(const Sha256_t& transaction_hash) const;

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
            uint32_t short_id, ShortIDs_t &dup_sids
    );

    void clear();

    TxProcessingResult_t process_transaction_msg(
        const Sha256_t& transaction_hash,
        PTxContents_t transaction_contents,
        unsigned int short_id,
        double timestamp,
        double current_time,
        bool enable_transaction_validation,
        uint64_t min_tx_network_fee,
        bool from_relay
    );

    TxFromBdnProcessingResult_t process_gateway_transaction_from_bdn(
        const Sha256_t& transaction_hash,
        PTxContents_t transaction_contents,
        unsigned int short_id,
        bool is_compact
    );

    PByteArray_t process_gateway_transaction_from_node(
        PTxsMessageContents_t txs_message_contents,
        uint64_t min_tx_network_fee,
        bool enable_transaction_validation
    );

    PByteArray_t get_transactions_by_short_ids(const SearializedShortIds_t& msg);

    PByteArray_t process_txs_msg(const TxsMsg_t& msg);

    PByteArray_t process_tx_sync_message(PTxContents_t tx_sync_msg);

private:

    size_t _final_tx_confirmations_count;
    Containers _containers;
    std::string _protocol;
    const AbstractMessageParser_t &_message_parser;
    const AbstractTransactionValidator_t &_tx_validation;

    std::tuple<TxStatus_t , TxValidationStatus_t> _msg_tx_build_tx_status(
        unsigned int short_id,
        const Sha256_t& transaction_hash,
        const PTxContents_t& transaction_contents,
        double timestamp,
        double current_time,
        bool enable_transaction_validation,
        uint64_t min_tx_network_fee,
        bool from_relay
    );

    const AbstractMessageParser_t& _create_message_parser() const;
    const AbstractTransactionValidator_t & _create_transaction_validator() const;
};


} // service


#endif /* TPE_SERVICE_TRANSACTION_SERVICE_H_ */
