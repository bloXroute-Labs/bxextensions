/*
    Taken from - https://github.com/FISCO-BCOS/FISCO-BCOS/blob/master/libdevcrypto/Hash.h
*/

#include "utils/crypto/sha256.h"

#ifndef UTILS_CRYPTO_KECCAK_H_
#define UTILS_CRYPTO_KECCAK_H_

namespace utils {
namespace crypto {

Sha256 keccak_sha3(const uint8_t *data, size_t from, size_t length) noexcept;

} // crypto
} // utils

#endif //UTILS_CRYPTO_KECCAK_H_
