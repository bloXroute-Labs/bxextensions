#include "tpe/task/sub_task/btc_compact_block_compression_sub_task.h"

#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_message_helper.h>
#include <utils/common/string_helper.h>

namespace task {

typedef utils::crypto::Sha256Context Sha256Context_t;
typedef service::TransactionPlaceholder TransactionPlaceholder_t;
typedef service::TransactionPlaceholderType TransactionPlaceholderType_t;

BtcCompactBlockCompressionSubTask::BtcCompactBlockCompressionSubTask(
    const CompactTransactionPlaceholders_t& tx_placeholders,
    const RecoveredTransactions_t& recovered_transactions,
    const std::vector<uint8_t>& block_header
):
    _tx_placeholders(tx_placeholders),
    _recovered_transactions(recovered_transactions),
    _block_header(block_header),
    _payload_length(0),
    _checksum(0)
{
}

uint32_t BtcCompactBlockCompressionSubTask::payload_length() {
	assert_execution();
	return _payload_length;
}
uint32_t BtcCompactBlockCompressionSubTask::checksum() {
	assert_execution();
	return _checksum;
}

void BtcCompactBlockCompressionSubTask::_execute() {
	Sha256Context_t sha_ctx;
	sha_ctx.update(_block_header);
	_payload_length = _block_header.size();
	auto recovered_iter = _recovered_transactions.begin();
	for (const TransactionPlaceholder_t& placeholder: _tx_placeholders)
	{
		PTxBuffer_t contents = nullptr;
		if (placeholder.type == TransactionPlaceholderType_t::missing_compact_id) {
			contents = *recovered_iter;
			++recovered_iter;
		} else {
			contents = placeholder.contents;
		}
		sha_ctx.update(*contents);
		_payload_length += contents->size();

	}
	Sha256_t sha = sha_ctx.digest();
	sha.double_sha256();
	utils::common::get_little_endian_value<uint32_t>(
	    sha.binary(), _checksum, 0
	);
}

} // task
