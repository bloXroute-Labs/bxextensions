#include <chrono>
#include <math.h>

#include <utils/common/buffer_view.h>
#include <utils/common/buffer_helper.h>
#include <utils/protocols/ontology/ont_message_parser.h>
#include <utils/protocols/ethereum/eth_message_parser.h>
#include <utils/protocols/bitcoin/btc_transaction_validator.h>
#include <utils/protocols/ethereum/eth_transaction_validator.h>
#include <utils/protocols/ontology/ont_transaction_validator.h>
#include "tpe/service/transaction_service.h"

#define HEADER_LENGTH 20 // global message header
#define NETWORK_NUM_LEN 4 // sizeof(uint_32_t)
#define TX_COUNT_LEN 4 // sizeof(uint_32_t)
#define SHA256_LEN 32
#define EXPIRATION_DATE_LEN 4 // sizeof(uint_32_t)
#define CONTENT_LEN 4 // sizeof(uint_32_t)
#define SHORT_IDS_COUNT_LEN 2
#define SHORT_ID_LEN 4 // sizeof(uint_32_t)
#define QUOTA_TYPE_LEN 1
#define SEEN_FLAG_LEN 1
#define HAS_TX_HASH_LEN 1

typedef utils::protocols::ontology::OntMessageParser OntMessageParser_t;
typedef utils::protocols::ethereum::EthMessageParser EthMessageParser_t;
typedef utils::protocols::ParsedTransaction_t ParsedTransaction_t;
typedef utils::protocols::ParsedTransactions_t ParsedTransactions_t;
typedef utils::protocols::bitcoin::BtcTransactionValidator BtcTransactionValidator_t;
typedef utils::protocols::ethereum::EthTransactionValidator EthTransactionValidator_t;
typedef utils::protocols::ontology::OntTransactionValidator OntTransactionValidator_t;

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

float TransactionService::get_short_id_assign_time(unsigned int short_id){
    return _containers.short_id_to_assign_time[short_id];
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

    if ( !has_short_id(short_id) ) {
        _containers.tx_hash_to_short_ids[transaction_hash].insert(short_id);
        _containers.short_id_to_tx_hash[short_id] = std::make_shared<Sha256_t>(transaction_hash);
    }

    float current_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    _containers.short_id_to_assign_time.emplace(short_id, current_time);

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
        _containers.tx_hash_to_contents.emplace(transaction_hash, std::move(transaction_contents));
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
            _containers.short_id_to_assign_time.erase(short_id);
        }
        _containers.tx_hash_to_short_ids.erase(sha);
    }
    auto content_iter = _containers.tx_hash_to_contents.find(sha);
    size_t content_length = 0;
    if (content_iter != _containers.tx_hash_to_contents.end()) {
        content_length = content_iter->second->size();
        _containers.tx_hash_to_contents.erase(content_iter);
    }
    return content_length;
}

size_t TransactionService::remove_transaction_by_short_id(
        uint32_t short_id, ShortIDs_t &dup_sids
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
                for (const uint32_t& dup_short_id : short_ids) {
                    _containers.short_id_to_tx_hash.erase(dup_short_id);
                    _containers.short_id_to_assign_time.erase(dup_short_id);
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

        _containers.short_id_to_tx_hash.erase(sha_iter);
        _containers.short_id_to_assign_time.erase(short_id);
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
    unsigned int current_time,
    std::string protocol,
    bool enable_transaction_validation
)
{
    unsigned int tx_status, tx_validation_status;
    TxShortIds_t existing_short_ids;
    AssignShortIDResult_t assign_short_id_result = 0;
    SetTransactionContentsResult_t set_transaction_contents_result;
    bool contents_set = false;
    bool short_id_assigned = false;

    std::tie(tx_status, tx_validation_status) = _msg_tx_build_tx_status(
        short_id,
        transaction_hash,
        transaction_contents,
        timestamp,
        current_time,
        protocol,
        enable_transaction_validation
    );

    if (
        has_status_flag(tx_validation_status, TX_VALIDATION_STATUS_INVALID_FORMAT) or
        has_status_flag(tx_validation_status, TX_VALIDATION_STATUS_INVALID_SIGNATURE) or
        has_status_flag(tx_status, TX_STATUS_IGNORE_SEEN) or
        has_status_flag(tx_status, TX_STATUS_TIMED_OUT)
    ) {
        TxProcessingResult_t result(tx_status, tx_validation_status);
        return std::move(result);
    }

    if ( has_status_flag(tx_status, TX_STATUS_SEEN_HASH) ) {
        existing_short_ids = _containers.tx_hash_to_short_ids[transaction_hash];
    }

    if (
        has_status_flag( tx_status, TX_STATUS_MSG_HAS_CONTENT ) and
        ! has_status_flag(tx_status, TX_STATUS_SEEN_CONTENT)
    ) {
        set_transaction_contents_result = set_transaction_contents(transaction_hash, std::move(transaction_contents));
        contents_set = true;
    }

    if ( has_status_flag(tx_status, TX_STATUS_MSG_HAS_SHORT_ID) ) {
        assign_short_id_result = assign_short_id(transaction_hash, short_id);
        short_id_assigned = true;
    }

    TxProcessingResult_t result(
            tx_status,
            tx_validation_status,
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
    SetTransactionContentsResult_t set_contents_result;

    bool existing_short_id = short_id != NULL_TX_SID and has_short_id(short_id);
    bool existing_contents = has_transaction_contents(transaction_hash);

    if (short_id == NULL_TX_SID and has_short_id(transaction_hash) and existing_contents) {
        return TxFromBdnProcessingResult_t(
            true,
            ignore_seen_short_id,
            assigned_short_id,
            existing_contents,
            set_content,
            std::move(set_contents_result)
        );
    }

    if ((existing_contents or is_compact) and existing_short_id) {
        return TxFromBdnProcessingResult_t(
            ignore_seen_contents,
            true,
            assigned_short_id,
            existing_contents,
            set_content,
            std::move(set_contents_result)
        );
    }

    if (short_id != NULL_TX_SID and not has_short_id(short_id)) {
        assign_short_id(transaction_hash, short_id);
        assigned_short_id = true;
    }

    if (not is_compact and not existing_contents) {
        set_contents_result = set_transaction_contents(transaction_hash, std::move(transaction_contents));
        set_content = true;
    }

    return TxFromBdnProcessingResult_t(
            ignore_seen_contents,
            ignore_seen_short_id,
            assigned_short_id,
            existing_contents,
            set_content,
            std::move(set_contents_result)
    );
}

PByteArray_t TransactionService::process_gateway_transaction_from_node(
    std::string protocol,
    PTxsMessageContents_t txs_message_contents
) {
    const AbstractMessageParser_t &message_parser = _create_message_parser(protocol);
    ParsedTransactions_t parsed_transactions = message_parser.parse_transactions_message(txs_message_contents);

    size_t total_result_size = 0;

    for (const ParsedTransaction_t &parsed_transaction: parsed_transactions) {
        total_result_size += parsed_transaction.length;
    }
    total_result_size += TX_COUNT_LEN + parsed_transactions.size() * (SHA256_LEN + SEEN_FLAG_LEN);

    PByteArray_t result_buffer = std::make_shared<ByteArray_t>(total_result_size);

    size_t offset = 0;
    offset = utils::common::set_little_endian_value(*result_buffer, uint32_t(parsed_transactions.size()), offset);

    for (const ParsedTransaction_t &parsed_transaction: parsed_transactions) {
        const Sha256_t &transaction_hash = parsed_transaction.transaction_hash;
        TxsMessageContents_t message_contents = *txs_message_contents;

        const bool seen_transaction = has_transaction_contents(transaction_hash);

        if (!seen_transaction) {
            ParsedTxContents_t tx_contents_copy = ParsedTxContents_t(
                    message_contents,
                    parsed_transaction.length,
                    parsed_transaction.offset);

            set_transaction_contents(
                    transaction_hash,
                    std::move(std::make_shared<BufferCopy_t>(std::move(tx_contents_copy)))
            );
        }

        offset = utils::common::set_little_endian_value(*result_buffer, uint8_t(seen_transaction), offset);
        offset = result_buffer->copy_from_buffer(transaction_hash.binary(), offset, 0, transaction_hash.size());
        offset = utils::common::set_little_endian_value(*result_buffer, uint32_t(parsed_transaction.offset), offset);
        offset = utils::common::set_little_endian_value(*result_buffer, uint32_t(parsed_transaction.length), offset);
    }

    // remove all items from the vector to make sure that it is properly disposed
    parsed_transactions.clear();

    result_buffer->set_output();
    return std::move(result_buffer);
}

PByteArray_t TransactionService::get_transactions_by_short_ids(const SearializedShortIds_t& msg) {
    ShortIDs_t short_ids;
    size_t input_msg_offset = 0;
    size_t short_ids_count = 0;

    input_msg_offset = utils::common::get_little_endian_value<uint32_t>(msg, short_ids_count, input_msg_offset);

    for (size_t i = 0; i < short_ids_count; ++i) {
        unsigned int short_id;
        input_msg_offset = utils::common::get_little_endian_value<uint32_t>(msg, short_id, input_msg_offset);
        short_ids.push_back(short_id);
    }

    // Buffer size can't be calculated ahead of time. Will be resizing buffer for each transaction
    size_t found_buffer_size = CONTENT_LEN + TX_COUNT_LEN;
    size_t missing_buffer_size = TX_COUNT_LEN;
    // Found transactions count is not known initially. Skip transactions info length and transactions count for now.
    size_t found_offset = found_buffer_size;
    size_t missing_offset = missing_buffer_size;
    size_t found_count = 0;
    size_t missing_count = 0;

    PByteArray_t result_buffer = std::make_shared<ByteArray_t>(found_buffer_size);
    ByteArray_t missing_buffer = ByteArray_t(missing_buffer_size);

    for (unsigned int short_id : short_ids) {
        bool found_short_id = has_short_id(short_id);
        if (found_short_id) {
            const Sha256_t& tx_hash = *_containers.short_id_to_tx_hash[short_id];

            bool has_contents = has_transaction_contents(tx_hash);

            if (has_contents) {
                PTxContents_t contents = _containers.tx_hash_to_contents[tx_hash];

                found_buffer_size += SHORT_ID_LEN + SHA256_LEN + CONTENT_LEN + contents->size();
                result_buffer->resize(found_buffer_size);

                found_offset = utils::common::set_little_endian_value(*result_buffer, uint32_t(short_id), found_offset);
                found_offset = result_buffer->copy_from_buffer(tx_hash.binary(), found_offset, 0, tx_hash.size());
                found_offset = utils::common::set_little_endian_value(*result_buffer, uint32_t(contents->size()), found_offset);
                found_offset = result_buffer->copy_from_buffer(*contents, found_offset, 0, contents->size());

                ++found_count;
                continue;
            } else {
                missing_buffer_size += SHORT_ID_LEN + HAS_TX_HASH_LEN + SHA256_LEN;
                missing_offset = utils::common::set_little_endian_value(missing_buffer, uint32_t(short_id), missing_offset);
                missing_offset = utils::common::set_little_endian_value(missing_buffer, uint8_t(1), missing_offset);
                missing_offset = missing_buffer.copy_from_buffer(tx_hash.binary(), missing_offset, 0, tx_hash.size());
                ++missing_count;
            }
        } else {
            missing_buffer_size += SHORT_ID_LEN + HAS_TX_HASH_LEN;
            missing_offset = utils::common::set_little_endian_value(missing_buffer, uint32_t(short_id), missing_offset);
            missing_offset = utils::common::set_little_endian_value(missing_buffer, uint8_t(0), missing_offset);
            ++missing_count;
        }
    }

    utils::common::set_little_endian_value(*result_buffer, uint32_t(found_buffer_size - CONTENT_LEN), 0);
    utils::common::set_little_endian_value(*result_buffer, uint32_t(found_count), TX_COUNT_LEN);

    utils::common::set_little_endian_value(missing_buffer, uint32_t(missing_count), 0);

    result_buffer->resize(found_buffer_size + missing_buffer_size);
    result_buffer->copy_from_array(missing_buffer.array(), found_offset, 0, missing_buffer.size());

    result_buffer->set_output();
    return std::move(result_buffer);
}

PByteArray_t TransactionService::process_txs_msg(const TxsMsg_t& msg) {
    // return a buffer that consists of
    // 2 bytes - number of missing transactions
    // list of:
    //      short-id uint32 (4 bytes)
    //      sha256 (32 bytes)
    //      content length of the transaction that was added to txService (4 bytes)

    size_t offset = HEADER_LENGTH;
    size_t output_buff_offset = SHORT_IDS_COUNT_LEN;
    uint16_t missing_short_ids = 0;

    uint32_t txn_count;
    offset = utils::common::get_little_endian_value<uint32_t>(msg, txn_count, offset);
    PByteArray_t buffer = std::make_shared<ByteArray_t>(SHORT_IDS_COUNT_LEN + (txn_count * (SHA256_LEN + SHORT_ID_LEN + CONTENT_LEN)));

    for (size_t tx = 0; tx < txn_count; ++tx) {
        unsigned int short_id = 0;
        bool missing = false;
        uint32_t content_length = 0;
        offset = utils::common::get_little_endian_value<uint32_t>(msg, short_id, offset);

        Sha256_t tx_hash(msg, offset);
        offset += SHA256_LEN;

        offset = utils::common::get_little_endian_value<uint32_t>(msg, content_length, offset);

        if (! has_short_id(short_id)) {
           assign_short_id(tx_hash, short_id);
           missing = true;
        }

        if (! has_transaction_contents(tx_hash) & content_length > 0) {
            BufferCopy_t transaction_content(TxContents_t(msg, content_length, offset));
            set_transaction_contents(
                tx_hash,
                std::move(std::make_shared<BufferCopy_t>(std::move(transaction_content)))
            );
            offset += content_length;
            missing = true;
        }
        else {
            offset += content_length;
            content_length = 0;
        }

        if (missing) {
            output_buff_offset = utils::common::set_little_endian_value<uint32_t>(*buffer, short_id, output_buff_offset);
            output_buff_offset = buffer->copy_from_buffer(tx_hash.binary(), output_buff_offset, 0, SHA256_LEN);
            output_buff_offset = utils::common::set_little_endian_value<uint32_t>(*buffer, content_length, output_buff_offset);
            ++missing_short_ids;
        }
    }

    utils::common::set_little_endian_value<uint16_t>(*buffer, missing_short_ids, 0);

    buffer->resize(output_buff_offset);
    buffer->set_output();
    return std::move(buffer);
}


std::tuple<TxStatus_t , TxValidationStatus_t> TransactionService::_msg_tx_build_tx_status(
    unsigned int short_id,
    const Sha256_t& transaction_hash,
    const PTxContents_t& transaction_contents,
    unsigned int timestamp,
    unsigned int current_time,
    std::string protocol,
    bool enable_transaction_validation
)
{
    unsigned int tx_status;
    unsigned int tx_validation_status = 0;

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

    if (
        timestamp != NULL_TX_TIMESTAMP
        and current_time - timestamp > MAX_TRANSACTION_ELAPSED_TIME_S
    ) {
        tx_status |= TX_STATUS_TIMED_OUT;
    }
    if ( has_status_flag(tx_status, TX_STATUS_MSG_HAS_CONTENT) and enable_transaction_validation ) {
        const AbstractTransactionValidator_t &tx_validation = _create_transaction_validator(protocol);
        tx_validation_status = tx_validation.transaction_validation(transaction_contents);
    }

    return std::make_tuple(tx_status, tx_validation_status);
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

const AbstractTransactionValidator_t& TransactionService::_create_transaction_validator(std::string protocol) const
{
    if (protocol == "bitcoin" or protocol == "bitcoincash") {
        static const BtcTransactionValidator_t transaction_validator = BtcTransactionValidator_t();
        return transaction_validator;
    }

    if (protocol == "ethereum") {
        static const EthTransactionValidator_t transaction_validator = EthTransactionValidator_t();
        return transaction_validator;
    }

    if (protocol == "ontology") {
        static const OntTransactionValidator_t transaction_validator = OntTransactionValidator_t();
        return transaction_validator;
    }

    throw std::runtime_error("Transaction validator is not available for provided protocol");
}
} // service
