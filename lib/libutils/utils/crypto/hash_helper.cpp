#include <cassert>

#include <openssl/sha.h>

#include "utils/crypto/hash_helper.h"
#include "utils/common/string_helper.h"


namespace utils {
namespace crypto {

Sha256 sha256(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length
)
{
	assert(data.size() >= from + length);
	return std::move(Sha256(data, from, length));
}

Sha256 double_sha256(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length
)
{
	Sha256 hash = std::move(sha256(data, from, length));
	hash.double_sha256();
	return std::move(hash);
}

size_t sha256_size() {
	return SHA256_DIGEST_LENGTH;
}

} // crypto
} // utils
