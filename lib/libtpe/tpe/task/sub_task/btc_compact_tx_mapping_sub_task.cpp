#include <utils/crypto/hash_helper.h>
#include <utils/concurrency/atomic_operation_helper.h>
#include <utils/common/buffer_copy.h>

#include "tpe/task/sub_task/btc_compact_tx_mapping_sub_task.h"
#include "tpe/consts.h"

namespace task {

typedef utils::crypto::Sha256 Sha256_t;
typedef utils::common::BufferCopy BufferCopy_t;

BtcCompactTxMappingSubTask::BtcCompactTxMappingSubTask():
		_compact_short_id_map(nullptr),
		_unknown_indices(nullptr),
		_tx_service(nullptr),
		_bucket(nullptr),
		_transaction_placeholders(nullptr),
		_counter(nullptr),
		_key(nullptr)
{
}

void BtcCompactTxMappingSubTask::init(
		CompactShortIdToShortIdMap_t* compact_short_id_map,
		UnknownTxIndices_t* unknown_indices,
		const TransactionService_t* tx_service,
		const Sha256Bucket_t* bucket,
		CompactTransactionPlaceholders_t* transaction_placeholders,
		CompactShortIdsCounter_t* counter,
		const utils::crypto::SipKey_t* key
)
{
	_compact_short_id_map = compact_short_id_map;
	_unknown_indices = unknown_indices;
	_tx_service = tx_service;
	_bucket = bucket;
	_transaction_placeholders = transaction_placeholders;
	_counter = counter;
	_key = key;
}

void BtcCompactTxMappingSubTask::_execute() {
	uint64_t total_count = _compact_short_id_map->size();
	const auto& sha_map = _tx_service->tx_hash_to_short_ids();
	const auto& content_map = _tx_service->tx_hash_to_contents();
	for (const Sha256_t& sha: *_bucket) {
	    auto sha_iter = sha_map.find(sha);
	    if (sha_iter == sha_map.end()) {
            continue;
	    }
		uint32_t short_id = *sha_iter->second.begin();
		CompactShortId_t compact_short_id = std::move(
				utils::crypto::to_compact_id(sha, *_key)
		);
		auto compact_iter = _compact_short_id_map->find(compact_short_id);
		if (compact_iter != _compact_short_id_map->end()) {
			std::pair<uint32_t, size_t>& compact_pair = compact_iter->second;
			uint32_t& src_short_id = compact_pair.first;
			TransactionPlaceholder_t& placeholder =
					_transaction_placeholders->at(compact_pair.second);
			uint32_t map_sid = null_short_id;
			bool duplicate_found = !utils::concurrency::compare_and_swap(
					src_short_id, map_sid, short_id
			);
			auto content_iter = content_map.find(sha);
			if (duplicate_found || content_iter == content_map.end()) {
				_unknown_indices->push_back(compact_pair.second);
				placeholder.type =
						service::TransactionPlaceholderType::missing_compact_id;
				placeholder.sid = null_short_id;
				if (duplicate_found) {
                    _counter->fetch_add(-1);
				}
			} else {
				placeholder.type = service::TransactionPlaceholderType::short_id;
				placeholder.sid = short_id;

                placeholder.contents = std::make_shared<BufferCopy_t>(
                        *content_iter->second
                );
                _counter->fetch_add(1);
			}
		}
		if (_counter->load() >= total_count) {
			break;
		}
	}
	_compact_short_id_map = nullptr;
	_unknown_indices = nullptr;
	_tx_service = nullptr;
	_bucket = nullptr;
	_transaction_placeholders = nullptr;
	_key = nullptr;
}

// TODO: Need to take this from the BtcCompactBlockMessage
const uint32_t BtcCompactTxMappingSubTask::null_short_id = NULL_TX_SID;

} // task




