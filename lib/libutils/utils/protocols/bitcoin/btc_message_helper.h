#include <iostream>
#include <vector>
#include <cstdint>
#include <utility>

#include "utils/crypto/hash_helper.h"
#include "utils/common/buffer_helper.h"
#include "utils/protocols/bitcoin/btc_consts.h"

#ifndef UTILS_PROTOCOLS_BITCOIN_BTC_MESSAGE_HELPER_H_
#define UTILS_PROTOCOLS_BITCOIN_BTC_MESSAGE_HELPER_H_

namespace utils {
namespace protocols {
namespace bitcoin {

template <class TBuffer>
size_t get_varint(
        const TBuffer& buffer,
		uint64_t & out_value,
		size_t offset
)
{
	uint8_t typ = buffer[offset];
	++offset;
	switch (typ) {
		case BTC_VARINT_LONG_INDICATOR:
			offset = common::get_little_endian_value<uint64_t>(
					buffer,
					out_value,
					offset
			);
			break;

		case BTC_VARINT_INT_INDICATOR:
			offset = common::get_little_endian_value<uint32_t>(
					buffer,
					out_value,
					offset
			);
			break;

		case BTC_VARINT_SHORT_INDICATOR:
			offset = common::get_little_endian_value<uint16_t>(
					buffer,
					out_value,
					offset
			);
			break;

		default:
			offset = common::get_little_endian_value<uint8_t>(
					buffer,
					out_value,
					offset - 1
			);
			break;
	}
	return offset;
}

template <class TBuffer>
size_t set_varint(
        TBuffer& buffer,
		uint64_t value,
		size_t offset
)
{
	if (value < BTC_VARINT_BYTE_MAX_SIZE) {
		return common::set_little_endian_value<uint8_t>(
				buffer, (uint8_t)value, offset
		);
	} else if (value <= BTC_VARINT_SHORT_MAX_SIZE) {
		offset = common::set_little_endian_value<uint8_t>(
				buffer, BTC_VARINT_SHORT_INDICATOR, offset
		);
		return common::set_little_endian_value<unsigned short>(
				buffer, (unsigned short)value, offset
		);
	} else if (value <= BTC_VARINT_INT_MAX_SIZE) {
		offset = common::set_little_endian_value<uint8_t>(
				buffer, BTC_VARINT_INT_INDICATOR, offset
		);
		return common::set_little_endian_value<uint32_t>(
				buffer, (uint32_t)value, offset
		);
	} else {
		offset = common::set_little_endian_value<uint8_t>(
				buffer, BTC_VARINT_LONG_INDICATOR, offset
		);
		return common::set_little_endian_value<uint64_t>(
				buffer, value, offset
		);
	}
}


template <class TBuffer>
crypto::Sha256 generate_block_hash(
        const TBuffer& buffer
)
{
	return std::move(
			crypto::double_sha256(
					buffer,
					BTC_HDR_COMMON_OFFSET,
					BTC_BLOCK_HDR_SIZE
			));
}

template <class TBuffer>
crypto::Sha256 get_prev_block_hash(
        const TBuffer& buffer
)
{
	crypto::Sha256 sha(buffer, BTC_PREV_BLOCK_OFFSET);
	sha.reverse();
	return std::move(sha);
}

template <class TBuffer>
std::pair<bool, size_t> get_segwit(
        const TBuffer& data,
        size_t offset
)
{
    uint16_t segwit_flag; // not initializing, since it will be passed by ref in the following line.
    size_t segwit_offset = common::get_big_endian_value<uint16_t> (
            data,
            segwit_flag, // by ref
            offset
    );
    bool is_segwit = segwit_flag == BTC_TX_SEGWIT_FLAG_VALUE;
    if (is_segwit) {
        offset = segwit_offset;
    }
    return std::make_pair(is_segwit, offset);
}

template <class TBuffer>
crypto::Sha256 get_tx_id(
        const TBuffer& data,
        size_t offset,
        size_t witness_offset,
        size_t end
)
{
    crypto::Sha256Context ctx;
    ctx.update(data, offset, BTC_TX_VERSION_SIZE);
    auto segwit = get_segwit(data, offset + BTC_TX_VERSION_SIZE);
    offset = segwit.second;
    ctx.update(data, offset, witness_offset - offset);
    ctx.update(data, end - BTC_TX_LOCK_TIME_SIZE, BTC_TX_LOCK_TIME_SIZE);
    crypto::Sha256 tx_hash = std::move(ctx.digest());
    tx_hash.double_sha256();
    tx_hash.reverse();
    return std::move(tx_hash);
}

} // bitcoin
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_MESSAGE_HELPER_H_ */
