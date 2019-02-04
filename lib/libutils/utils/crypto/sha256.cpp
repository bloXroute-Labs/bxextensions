#include <algorithm>
#include <openssl/sha.h>

#include "utils/crypto/sha256.h"

namespace utils {
namespace crypto {


static void calculate_sha256(const std::vector<uint8_t>& data,
		size_t from,
		size_t length,
		std::vector<uint8_t>& out_sha256) {
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, &data[from], length);
	SHA256_Final(&out_sha256[0], &sha256);
}

Sha256::Sha256(): _sha256(SHA256_DIGEST_LENGTH) {
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
}

Sha256::Sha256(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length): _sha256(SHA256_DIGEST_LENGTH) {
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	calculate_sha256(data, from, length, _sha256);
}

Sha256::Sha256(Sha256&& other) {
	_sha256 = std::move(other._sha256);
}

const Sha256& Sha256::operator++() {
	calculate_sha256(_sha256, 0,
			SHA256_DIGEST_LENGTH,
			_sha256);
	return *this;
}

void Sha256::operator()(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length) {
	calculate_sha256(data, from, length, _sha256);
}

bool Sha256::operator==(const Sha256& other) const {
	return std::equal(
			_sha256.begin(),
			_sha256.end(),
			other._sha256.begin());
}

const std::vector<uint8_t>& Sha256::sha256() {
	return _sha256;
}

size_t Sha256::size() const {
	return SHA256_DIGEST_LENGTH;
}

size_t Sha256::hash() const{
	return *(const_cast<size_t *>(
			reinterpret_cast<const size_t*>(&_sha256[0])
			));
}

size_t Sha256Hasher::operator ()(const Sha256& key) const {
	return key.hash();
}

bool Sha256Equal::operator()(const Sha256& lhs, const Sha256& rhs) const {
	return lhs == rhs;
}

} // crypto
} // utils
