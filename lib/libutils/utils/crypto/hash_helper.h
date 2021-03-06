#include <iostream>
#include <cstdint>
#include <vector>
#include <cassert>

#include "utils/crypto/sha256.h"
#include "utils/crypto/compact_short_id.h"
#include "utils/crypto/keccak.h"

#ifndef UTILS_CRYPTO_HASH_HELPER_H_
#define UTILS_CRYPTO_HASH_HELPER_H_

namespace utils {
namespace crypto {

template<class TBuffer>
Sha256 sha256(
		const TBuffer& data,
		size_t from,
		size_t length
)
{
//	assert(data.size() >= from + length);
    if (data.size() < from + length) {
        throw std::runtime_error("invalid buffer size provided"); // TODO : replace with proper exception here
    }
	return Sha256(data, from, length);
}

template<class TBuffer>
Sha256 double_sha256(
		const TBuffer& data,
		size_t from,
		size_t length
)
{
	Sha256 hash = std::move(sha256(data, from, length));
	hash.double_sha256();
	hash.reverse();
	return hash;
}

template<class TBuffer, typename... Args>
Sha256 several_sources_sha256(
    const TBuffer& data,
    FromLengthPair_t source,
    Args... extra_sources
)
{
    Sha256 hash = Sha256(data, {source, extra_sources...});
    return hash;
}

size_t sha256_size();

CompactShortId to_compact_id(
		const Sha256& sha, const SipKey_t& key
);

template <class TBuffer>
Sha256 sha3(const TBuffer& data, size_t from, size_t length) {
    return std::move(keccak_sha3(&data.at(0), from,  length));
}

} // crypto
} // utils

#endif /* UTILS_CRYPTO_HASH_HELPER_H_ */
