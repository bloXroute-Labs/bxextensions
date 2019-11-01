#include <iostream>

#include "tpe/service/transaction_placeholder.h"
#include "tpe/service/transaction_to_short_ids_map.h"

#include <utils/protocols/bitcoin/btc_compact_block_message.h>

#ifndef TPE_SERVICE_COMPACT_BLOCK_DATA_SERVICE_H_
#define TPE_SERVICE_COMPACT_BLOCK_DATA_SERVICE_H_

namespace service {

typedef std::vector<TransactionPlaceholder> CompactTransactionPlaceholders_t;
typedef utils::crypto::CompactShortId CompactShortId_t;
typedef utils::crypto::CompactShortIdMap_t<std::pair<unsigned int, size_t>> CompactShortIdToShortIdMap_t;
typedef utils::crypto::SipKey_t SipKey_t;
typedef utils::crypto::Sha256 Sha256_t;
typedef utils::protocols::bitcoin::BtcCompactBlockMessage BtcCompactBlockMessage_t;
typedef std::shared_ptr<BtcCompactBlockMessage_t> PBtcCompactBlockMessage_t;
typedef utils::common::BufferView BufferView_t;
typedef std::shared_ptr<BufferView_t> PBufferView_t;

class CompactBlockDataService {
public:

	CompactBlockDataService();

	CompactBlockDataService(
			PBufferView_t block_buffer,
			const Sha256ToShortIDsMap_t* short_id_map
	);
	CompactBlockDataService(CompactBlockDataService&& rhs) noexcept ;

	CompactBlockDataService& operator =(CompactBlockDataService&& rhs) noexcept ;

	void parse();

	const SipKey_t& sip_key() const;
	const BtcCompactBlockMessage_t& msg() const;
	size_t size() const;
	uint64_t total_tx_count() const;
	uint64_t compact_tx_count() const;

	size_t byte_size() const;

	CompactTransactionPlaceholders_t& tx_placeholders();
	CompactShortIdToShortIdMap_t& compact_map();

	void on_compression_completed();

private:

	uint64_t _fill_compact_ids(
			uint64_t compact_diff, size_t& offset, uint64_t idx
	);

	CompactTransactionPlaceholders_t _tx_placeholders;
	PBufferView_t _block_buffer;
	SipKey_t _sip_key;
	CompactShortIdToShortIdMap_t _compact_map;
	PBtcCompactBlockMessage_t _msg;
	const Sha256ToShortIDsMap_t* _short_id_map;
	uint64_t _total_tx_count, _compact_tx_count;
};

} // service

#endif /* TPE_SERVICE_COMPACT_BLOCK_DATA_SERVICE_H_ */
