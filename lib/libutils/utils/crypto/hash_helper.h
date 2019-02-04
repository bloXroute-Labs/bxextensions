#include <iostream>
#include <cstdint>
#include <vector>

#include "utils/crypto/sha256.h"

#ifndef UTILS_CRYPTO_HASH_HELPER_H_
#define UTILS_CRYPTO_HASH_HELPER_H_

namespace utils {
namespace crypto {

Sha256 sha256(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length
);

Sha256 double_sha256(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length
);

size_t sha256_size(void);

} // crypto
} // utils

#endif /* UTILS_CRYPTO_HASH_HELPER_H_ */
