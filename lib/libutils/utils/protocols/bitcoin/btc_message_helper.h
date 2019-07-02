#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

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
    const TBuffer& buffer)
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
    const TBuffer& buffer)
{
	crypto::Sha256 sha(buffer, BTC_PREV_BLOCK_OFFSET);
	sha.reverse();
	return std::move(sha);
}

} // bitcoin
} // protocols
} // utils

#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_MESSAGE_HELPER_H_ */
