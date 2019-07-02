#include <cassert>

#include <openssl/sha.h>

#include "utils/crypto/hash_helper.h"
#include "utils/common/string_helper.h"


namespace utils {
namespace crypto {

size_t sha256_size() {
	return SHA256_DIGEST_LENGTH;
}

CompactShortId to_compact_id(
		const Sha256& sha, const utils::crypto::SipKey_t& key
)
{
	return std::move(CompactShortId(key, sha));
}


} // crypto
} // utils
