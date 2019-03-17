#include <iostream>
#include <cstdint>
#include <vector>

#include "utils/crypto/sha256.h"

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
	assert(data.size() >= from + length);
	return std::move(Sha256(data, from, length));
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
	return std::move(hash);
}

size_t sha256_size(void);

} // crypto
} // utils

#endif /* UTILS_CRYPTO_HASH_HELPER_H_ */
