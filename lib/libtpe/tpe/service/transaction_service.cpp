#include <chrono>
#include <math.h>

#include <utils/common/buffer_view.h>
#include <utils/common/buffer_helper.h>
#include <utils/protocols/ontology/ont_message_parser.h>
#include <utils/protocols/ethereum/eth_message_parser.h>
#include "tpe/service/transaction_service.h"

#define NETWORK_NUM_LEN 4 // sizeof(uint_32_t)
#define TX_COUNT_LEN 4 // sizeof(uint_32_t)
#define SHA256_LEN 32
#define EXPIRATION_DATE_LEN 4 // sizeof(uint_32_t)
#define CONTENT_LEN 4 // sizeof(uint_32_t)
#define SHORT_IDS_COUNT_LEN 2
#define SHORT_ID_LEN 4 // sizeof(uint_32_t)
#define QUOTA_TYPE_LEN 1

typedef utils::protocols::ontology::OntMessageParser OntMessageParser_t;
typedef utils::protocols::ethereum::EthMessageParser EthMessageParser_t;
typedef utils::protocols::ParsedTransaction_t ParsedTransaction_t;
typedef utils::protocols::ParsedTransactions_t ParsedTransactions_t;

namespace service {

TransactionService::TransactionService(
        size_t pool_size,
        size_t tx_bucket_capacity/* = BTC_DEFAULT_TX_BUCKET_SIZE*/,
        size_t final_tx_confirmations_count/* = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT*/
):
        _containers(pool_size, tx_bucket_capacity),
		_final_tx_confirmations_count(final_tx_confirmations_count)
{
}

Sha256ToShortIDsMap_t& TransactionService::get_tx_hash_to_short_ids() {
	return _containers.tx_hash_to_short_ids;
}

ShortIDToSha256Map_t& TransactionService::get_short_id_to_tx_hash() {
	return _containers.short_id_to_tx_hash;
}

Sha256ToContentMap_t& TransactionService::get_tx_hash_to_contents() {
	return _containers.tx_hash_to_contents;
}

PTxSyncTxs_t TransactionService::get_tx_sync_buffer(size_t all_txs_content_size, bool sync_tx_content) {
    size_t total_tx_hashes = _containers.tx_hash_to_contents.size();
    size_t total_short_ids = _containers.short_id_to_tx_hash.size();

    size_t total_size = (TX_COUNT_LEN +
            (SHA256_LEN + EXPIRATION_DATE_LEN + CONTENT_LEN + SHORT_IDS_COUNT_LEN) * total_tx_hashes) +
            ((SHORT_ID_LEN + QUOTA_TYPE_LEN) * total_short_ids);

    if (sync_tx_content)
        total_size += all_txs_content_size;

    PTxSyncTxs_t buffer = std::make_shared<TxSyncTxs_t>(total_size);

    size_t current_pos = 0;

    current_pos = utils::common::set_little_endian_value(*buffer, uint32_t(total_tx_hashes), current_pos);

    // TODO: since this function is called in the main thread and all the updates are also done in the main thread, there is no race condition.
    // If moving this function to a task, need to check race condition
    for (const auto& tx_hash_to_contents : _containers.tx_hash_to_contents) {

        // pack hash
        current_pos = buffer->copy_from_buffer(tx_hash_to_contents.first.binary(), current_pos, 0, SHA256_LEN);

        // pack transaction content
        if (sync_tx_content) {
            uint32_t content_len = tx_hash_to_contents.second->size();
            current_pos = utils::common::set_little_endian_value(*buffer, content_len, current_pos);
            current_pos = buffer->copy_from_buffer(*tx_hash_to_contents.second.get(), current_pos, 0, content_len);
        } else {
            current_pos = utils::common::set_little_endian_value(*buffer, 0, current_pos);
        }

        // expiration date is not available in extension
        current_pos += EXPIRATION_DATE_LEN;

        const auto& tx_hash_to_short_id = _containers.tx_hash_to_short_ids.find(tx_hash_to_contents.first);

        if (tx_hash_to_short_id != _containers.tx_hash_to_short_ids.end()) {
            uint16_t short_ids = tx_hash_to_short_id->second.size();
            current_pos = utils::common::set_little_endian_value(*buffer, short_ids, current_pos);
            for (const auto& sid: tx_hash_to_short_id->second) {
                current_pos = utils::common::set_little_endian_value(*buffer, sid, current_pos);
            }
            // TODO: once extensions knows sid quota, need to enter the quota of each short id here
            current_pos += tx_hash_to_short_id->second.size();
        } else {
            current_pos += SHORT_IDS_COUNT_LEN;
        }
    }
    buffer->resize(current_pos);
    buffer->set_output();
    return std::move(buffer);
}

TxNotSeenInBlocks_t& TransactionService::tx_not_seen_in_blocks() {
    return _containers.tx_not_seen_in_blocks;
}

const Sha256ToShortIDsMap_t&
TransactionService::tx_hash_to_short_ids() const {
	return _containers.tx_hash_to_short_ids;
}

const Sha256ToContentMap_t&
TransactionService::tx_hash_to_contents() const {
	return _containers.tx_hash_to_contents;
}

AssignShortIDResult_t TransactionService::assign_short_id(const Sha256_t& transaction_hash, unsigned int short_id) {
    AssignShortIDResult_t result = has_transaction_contents(transaction_hash);

    if (short_id == NULL_TX_SID) {
        return std::move(result);
    }

    if (!has_short_id(short_id)) {
        _containers.tx_hash_to_short_ids[transaction_hash].insert(short_id);
        _containers.short_id_to_tx_hash[short_id] = std::make_shared<Sha256_t>(transaction_hash);
    }

    return std::move(result);
}

SetTransactionContentsResult_t TransactionService::set_transaction_contents(
            const Sha256_t& transaction_hash,
            PTxContents_t transaction_contents) {
    SetTransactionContentsResult_t result;

    result.first = has_short_id(transaction_hash);

    if (has_transaction_contents(transaction_hash)) {
        result.second = _containers.tx_hash_to_contents[transaction_hash]->size();
    } else {
        _containers.tx_hash_to_contents.emplace(transaction_hash, transaction_contents);
    }

    return std::move(result);
}

bool TransactionService::has_short_id(const Sha256_t& tx_hash) const {
	auto iter = _containers.tx_hash_to_short_ids.find(tx_hash);
	return iter != _containers.tx_hash_to_short_ids.end();
}

bool TransactionService::has_short_id(unsigned int short_id) const {
	auto iter = _containers.short_id_to_tx_hash.find(short_id);
	return iter != _containers.short_id_to_tx_hash.end();
}

bool TransactionService::has_transaction_contents(const Sha256_t& tx_hash) const {
    auto iter = _containers.tx_hash_to_contents.find(tx_hash);
    return iter != _containers.tx_hash_to_contents.end();
}

bool TransactionService::removed_transaction(const Sha256_t& tx_hash) const {
    auto iter = _containers.tx_hash_to_time_removed.find(tx_hash);
	return iter != _containers.tx_hash_to_time_removed.end();
}

Sha256ToTime_t&
TransactionService::tx_hash_to_time_removed() {
	return _containers.tx_hash_to_time_removed;
}

unsigned int TransactionService::get_short_id(
		const Sha256_t& tx_hash
) const {
	auto iter = _containers.tx_hash_to_short_ids.find(tx_hash);
	return *iter->second.begin();
}

PTxContents_t TransactionService::get_transaction(
		unsigned int short_id, Sha256_t& tx_hash
)
{
	tx_hash = *_containers.short_id_to_tx_hash[short_id];
	return _containers.tx_hash_to_contents[tx_hash];
}

const TxContents_t* TransactionService::get_tx_contents_raw_ptr(
        unsigned int short_id
)
{
    const Sha256_t& tx_hash = *_containers.short_id_to_tx_hash[short_id];
    return _containers.tx_hash_to_contents[tx_hash].get();
}

size_t TransactionService::size() const {
	return _containers.tx_hash_to_short_ids.size();
}

size_t TransactionService::get_tx_size(unsigned int short_id) const {
	Sha256_t& tx_hash = *_containers.short_id_to_tx_hash.at(short_id);
	return _containers.tx_hash_to_contents.at(tx_hash)->size();
}

bool TransactionService::get_missing_transactions(
		UnknownTxHashes_t& unknown_tx_hashes,
		ShortIDs_t& unknown_tx_sids,
		const std::vector<unsigned int>& short_ids
) const
{
	bool success = true;
	for (unsigned int short_id: short_ids) {
		auto sid_iter = _containers.short_id_to_tx_hash.find(short_id);
		if (sid_iter != _containers.short_id_to_tx_hash.end() && short_id != NULL_TX_SID) {
			auto hash_iter = _containers.tx_hash_to_contents.find(*sid_iter->second);
			if (hash_iter == _containers.tx_hash_to_contents.end()) {
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

const TxNotSeenInBlocks_t& TransactionService::acquire_tx_pool() {
    _containers.tx_not_seen_in_blocks.acquire_read();
	return _containers.tx_not_seen_in_blocks;
}

void TransactionService::on_finished_reading_tx_pool() {
    _containers.tx_not_seen_in_blocks.release_read();
}

void TransactionService::track_seen_transaction(const Sha256_t &sha) {
    _containers.tx_not_seen_in_blocks.erase(sha);
}

// TODO : add a lock to this function to prevent a race condition between data structures
TrackSeenResult_t TransactionService::track_seen_short_ids(
        const Sha256_t& block_hash, ShortIDs_t short_ids
)
{
    TrackSeenResult_t result;
    result.first = 0;
    _containers.short_ids_seen_in_block.emplace(block_hash, std::move(short_ids));
    for (const unsigned int& short_id: _containers.short_ids_seen_in_block[block_hash]) {
        auto sha_itr = _containers.short_id_to_tx_hash.find(short_id);
        if (sha_itr != _containers.short_id_to_tx_hash.end()) {
            _containers.tx_not_seen_in_blocks.erase(*sha_itr->second);
        }
    }
    if (_containers.short_ids_seen_in_block.size() >= _final_tx_confirmations_count) {
        auto final_short_ids_iter = _containers.short_ids_seen_in_block.begin();
        for (const unsigned int& short_id: final_short_ids_iter->second) {
            result.first +=
                    remove_transaction_by_short_id(short_id, result.second);
        }
        _containers.short_ids_seen_in_block.erase(final_short_ids_iter->first);
    }
    return std::move(result);
}

void TransactionService::on_block_cleaned_up(const Sha256_t& block_hash) {
    _containers.short_ids_seen_in_block.erase(block_hash);
}

void TransactionService::set_final_tx_confirmations_count(size_t val) {
    _final_tx_confirmations_count = val;
}

size_t TransactionService::remove_transactions_by_hashes(const std::vector<Sha256_t>& tx_hashes) {
    size_t  total_content_length = 0;
    for (const Sha256_t& sha: tx_hashes) {
        total_content_length += remove_transaction_by_hash(sha);
    }
    return total_content_length;
}

size_t TransactionService::remove_transaction_by_hash(const Sha256_t& sha) {
    auto short_ids_iter = _containers.tx_hash_to_short_ids.find(sha);
    if (short_ids_iter != _containers.tx_hash_to_short_ids.end()) {
        for (const uint32_t& short_id: short_ids_iter->second) {
            _containers.short_id_to_tx_hash.erase(short_id);
        }
        _containers.tx_hash_to_short_ids.erase(sha);
    }
    auto content_iter = _containers.tx_hash_to_contents.find(sha);
    size_t content_length = 0;
    if (content_iter != _containers.tx_hash_to_contents.end()) {
        content_length = content_iter->second->size();
        _containers.tx_hash_to_contents.erase(content_iter);
    }
    _containers.tx_hash_to_time_removed.emplace(sha, std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    return content_length;
}

size_t TransactionService::remove_transaction_by_short_id(
        unsigned int short_id, ShortIDs_t &dup_sids
)
{
    auto sha_iter = _containers.short_id_to_tx_hash.find(short_id);
    size_t contents_len = 0;
    if (sha_iter != _containers.short_id_to_tx_hash.end()) {
        const Sha256_t& sha = *sha_iter->second;
        auto short_ids_iter = _containers.tx_hash_to_short_ids.find(sha);
        if (short_ids_iter != _containers.tx_hash_to_short_ids.end()) {
            auto& short_ids = short_ids_iter->second;
            if (short_ids.size() > 1) {
                short_ids.erase(short_id);
                for (const unsigned int& dup_short_id : short_ids) {
                    _containers.short_id_to_tx_hash.erase(dup_short_id);
                    dup_sids.push_back(dup_short_id);
                }
            }
            _containers.tx_hash_to_short_ids.erase(sha);
        }
        auto content_iter = _containers.tx_hash_to_contents.find(sha);
        if (content_iter != _containers.tx_hash_to_contents.end()) {
            contents_len = content_iter->second->size();
            _containers.tx_hash_to_contents.erase(content_iter);
        }

        _containers.tx_hash_to_time_removed.emplace(sha, std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

        _containers.short_id_to_tx_hash.erase(sha_iter);
    }
    return contents_len;
}

void TransactionService::clear_short_ids_seen_in_block() {
    _containers.short_ids_seen_in_block.clear();
}

TxProcessingResult_t TransactionService::process_transaction_msg(
            const Sha256_t& transaction_hash,
            PTxContents_t transaction_contents,
            unsigned int network_num,
            unsigned int short_id,
            unsigned int timestamp,
            unsigned int current_time) {

    unsigned int tx_status;
    TxShortIds_t existing_short_ids;
    AssignShortIDResult_t assign_short_id_result;
    SetTransactionContentsResult_t set_transaction_contents_result;
    bool contents_set = false;
    bool short_id_assigned = false;

    tx_status = _msg_tx_build_tx_status(
        short_id,
        transaction_hash,
        transaction_contents,
        timestamp,
        current_time
    );

    if (tx_status & TX_STATUS_IGNORE_SEEN or tx_status & TX_STATUS_TIMED_OUT) {
        TxProcessingResult_t result(tx_status);
        return std::move(result);
    }

    if (tx_status & TX_STATUS_SEEN_HASH) {
        existing_short_ids = _containers.tx_hash_to_short_ids[transaction_hash];
    }

    if (tx_status & TX_STATUS_MSG_HAS_CONTENT and !(tx_status & TX_STATUS_SEEN_CONTENT)) {
        set_transaction_contents_result = set_transaction_contents(transaction_hash, transaction_contents);
        contents_set = true;
    }

    if (tx_status & TX_STATUS_MSG_HAS_SHORT_ID) {
        assign_short_id_result = assign_short_id(transaction_hash, short_id);
        short_id_assigned = true;
    }

    TxProcessingResult_t result(
            tx_status,
            existing_short_ids,
            assign_short_id_result,
            set_transaction_contents_result,
            contents_set,
            short_id_assigned);
    return std::move(result);
}

TxFromBdnProcessingResult_t TransactionService::process_gateway_transaction_from_bdn(
            const Sha256_t& transaction_hash,
            PTxContents_t transaction_contents,
            unsigned int short_id,
            bool is_compact
    ) {
        bool ignore_seen_contents = false;
        bool ignore_seen_short_id = false;
        bool assigned_short_id = false;
        bool set_content = false;

        bool existing_short_id = short_id != NULL_TX_SID and has_short_id(short_id);
        bool existing_contents = has_transaction_contents(transaction_hash);

        if ((short_id == NULL_TX_SID and has_short_id(transaction_hash) and existing_contents)
                or removed_transaction(transaction_hash)) {
            return TxFromBdnProcessingResult_t(
                true,
                ignore_seen_short_id,
                assigned_short_id,
                existing_contents,
                set_content
            );
        }

        if ((existing_contents or is_compact) and existing_short_id) {
            return TxFromBdnProcessingResult_t(
                ignore_seen_contents,
                true,
                assigned_short_id,
                existing_contents,
                set_content
            );
        }

        if (short_id != NULL_TX_SID and not has_short_id(short_id)) {
            assign_short_id(transaction_hash, short_id);
            assigned_short_id = true;
        }

        if (not is_compact and not existing_contents) {
            set_transaction_contents(transaction_hash, transaction_contents);
            set_content = true;
        }

        return TxFromBdnProcessingResult_t(
            ignore_seen_contents,
            ignore_seen_short_id,
            assigned_short_id,
            existing_contents,
            set_content
        );
}

PTxFromNodeProcessingResultList_t TransactionService::process_gateway_transaction_from_node(
            std::string protocol,
            PTxsMessageContents_t txs_message_contents
    ) {

    const AbstractMessageParser_t& message_parser = _create_message_parser(protocol);
    ParsedTransactions_t parsed_transactions = message_parser.parse_transactions_message(txs_message_contents);

    TxFromNodeProcessingResultList_t result;

    for (const ParsedTransaction_t& parsed_transaction: parsed_transactions) {
        const bool seen_transaction =
            has_transaction_contents(parsed_transaction.first)
            or removed_transaction(parsed_transaction.first);

        const TxContents_t& tx_buffer = parsed_transaction.second;
        const size_t tx_size = tx_buffer.size();

        if (!seen_transaction) {
            set_transaction_contents(
                parsed_transaction.first,
                std::make_shared<BufferCopy_t>(tx_buffer)
            );
        }

        //TODO: Need to try avoid second copy here
        PParsedTxContents_t tx_contents = std::make_shared<ParsedTxContents_t>(tx_size);
        tx_contents->copy_from_buffer(tx_buffer, 0, 0, tx_size);
        tx_contents->set_output();

        TxFromNodeProcessingResult_t tx_result = TxFromNodeProcessingResult_t(
                seen_transaction,
                std::make_shared<Sha256_t>(std::move(parsed_transaction.first)),
                tx_contents
        );
        result.push_back(tx_result);
    }

    // remove all items from the vector to make sure that it is properly disposed
    parsed_transactions.clear();

    return std::move(std::make_shared<TxFromNodeProcessingResultList_t>(std::move(result)));
}

unsigned int TransactionService::_msg_tx_build_tx_status(
    unsigned int short_id,
    const Sha256_t& transaction_hash,
    const PTxContents_t& transaction_contents,
    unsigned int timestamp,
    unsigned int current_time) {

    unsigned int tx_status = 0;
    tx_status = (short_id != NULL_TX_SID) ? TX_STATUS_MSG_HAS_SHORT_ID : TX_STATUS_MSG_NO_SHORT_ID;

    //set tx status flags, flag (right side of and clause) will be set if the condition in the left side is met
    if (transaction_contents->size() == 0) {
        tx_status |= TX_STATUS_MSG_NO_CONTENT;
    } else {
        tx_status |= TX_STATUS_MSG_HAS_CONTENT;
    }

    if (has_status_flag(tx_status, TX_STATUS_MSG_HAS_SHORT_ID) and has_short_id(short_id)) {
        tx_status |= TX_STATUS_SEEN_SHORT_ID;
    }
    if (has_short_id(transaction_hash)) {
        tx_status |= TX_STATUS_SEEN_HASH;
    }
    if (has_transaction_contents(transaction_hash)) {
        tx_status |= TX_STATUS_SEEN_CONTENT;
        tx_status |= TX_STATUS_SEEN_HASH;
    }
    if (removed_transaction(transaction_hash)) {
        tx_status |= TX_STATUS_SEEN_REMOVED_TRANSACTION;
    }

    // check all variations for previously seen Tx
    if (has_status_flag(tx_status, TX_STATUS_SEEN_CONTENT) and
        has_status_flag(tx_status, TX_STATUS_SEEN_HASH) and
        has_status_flag(tx_status, TX_STATUS_MSG_NO_SHORT_ID)) {

        tx_status |= TX_STATUS_IGNORE_SEEN;
    }
    if (has_status_flag(tx_status, TX_STATUS_SEEN_CONTENT) and
        has_status_flag(tx_status, TX_STATUS_SEEN_HASH) and
        has_status_flag(tx_status, TX_STATUS_SEEN_SHORT_ID)) {
        tx_status |= TX_STATUS_IGNORE_SEEN;
    }

    if (has_status_flag(tx_status, TX_STATUS_MSG_NO_CONTENT) and
        has_status_flag(tx_status, TX_STATUS_SEEN_HASH) and
        has_status_flag(tx_status,  TX_STATUS_MSG_NO_SHORT_ID)) {
        tx_status |= TX_STATUS_IGNORE_SEEN;
    }

    if (has_status_flag(tx_status, TX_STATUS_MSG_NO_CONTENT) and
        has_status_flag(tx_status, TX_STATUS_SEEN_HASH) and
        has_status_flag(tx_status, TX_STATUS_SEEN_SHORT_ID)) {
        tx_status |= TX_STATUS_IGNORE_SEEN;
    }

    if (has_status_flag(tx_status, TX_STATUS_SEEN_REMOVED_TRANSACTION)) {
        tx_status |= TX_STATUS_IGNORE_SEEN;
    }

    if (
        timestamp != NULL_TX_TIMESTAMP
        and current_time - timestamp > MAX_TRANSACTION_ELAPSED_TIME_S
    ) {
        tx_status |= TX_STATUS_TIMED_OUT;
    }
    return tx_status;
}

const AbstractMessageParser_t& TransactionService::_create_message_parser(std::string protocol) const {
    if (protocol == "ontology") {
        static const OntMessageParser_t message_converter = OntMessageParser_t();
        return message_converter;
    }

    if (protocol == "ethereum") {
        static const EthMessageParser_t message_converter = EthMessageParser_t();
        return message_converter;
    }

    throw std::runtime_error("Message converter is not available for provided protocol");
}

} // service
