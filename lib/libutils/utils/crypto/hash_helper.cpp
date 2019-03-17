#include <cassert>

#include <openssl/sha.h>

#include "utils/crypto/hash_helper.h"
#include "utils/common/string_helper.h"


namespace utils {
namespace crypto {

size_t sha256_size() {
	return SHA256_DIGEST_LENGTH;
}

} // crypto
} // utils
