#include <algorithm>
#include <openssl/sha.h>

#include "utils/crypto/sha256.h"
#include "utils/common/string_helper.h"

namespace utils {
namespace crypto {

static size_t get_hash(const std::vector<uint8_t>& sha256) {
	std::hash<uint8_t> hasher;
	size_t hash = 0;
	for (const auto& byte: sha256) {
		hash = hash * 31 + hasher(byte);
	}
	return hash;
}

static void calculate_sha256(const std::vector<uint8_t>& data,
		size_t from,
		size_t length,
		std::vector<uint8_t>& out_sha256,
		size_t& out_hash) {
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, &data[from], length);
	SHA256_Final(&out_sha256[0], &sha256);
	out_hash = get_hash(out_sha256);
}

Sha256::Sha256(): _sha256(SHA256_DIGEST_LENGTH), _hash(0) {
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
}

Sha256::Sha256(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length): _sha256(SHA256_DIGEST_LENGTH),
				_hash(0) {
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	calculate_sha256(data, from, length, _sha256, _hash);

}

Sha256::Sha256(const Sha256& other) {
	_sha256 = other._sha256;
	_hash = other._hash;
}

Sha256::Sha256(Sha256&& other) {
	_sha256 = std::move(other._sha256);
	_hash = other._hash;
}

const Sha256& Sha256::operator=(const Sha256& other) {
	_sha256 = other._sha256;
	_hash = other._hash;
	return *this;
}

void Sha256::double_sha256() {
	calculate_sha256(_sha256, 0,
			SHA256_DIGEST_LENGTH,
			_sha256,
			_hash);
	std::reverse(_sha256.begin(), _sha256.end());
	_hash = get_hash(_sha256);
}

void Sha256::clear() {
	_sha256.clear();
	_hash = 0;
}


std::ostream& operator <<(std::ostream& out, const Sha256& sha) {
	out << sha.repr();
	return out;
}

void Sha256::operator()(
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length) {
	calculate_sha256(data, from, length, _sha256, _hash);
}

bool Sha256::operator==(const Sha256& other) const {
	return std::equal(
			_sha256.begin(),
			_sha256.end(),
			other._sha256.begin());
}

const std::vector<uint8_t>& Sha256::sha256() const {
	return _sha256;
}

std::string Sha256::repr(void) const {
	return common::to_hex_string(_sha256);
}

size_t Sha256::size() const {
	return SHA256_DIGEST_LENGTH;
}

size_t Sha256::hash() const {
	return _hash;
}

size_t Sha256Hasher::operator ()(const Sha256& key) const {
	return key.hash();
}

bool Sha256Equal::operator()(const Sha256& lhs, const Sha256& rhs) const {
	return lhs == rhs;
}

} // crypto
} // utils
