#include <algorithm>
#include <openssl/sha.h>
#include <cstring>

#include "utils/crypto/sha256.h"
#include "utils/common/string_helper.h"
#include "utils/common/buffer_helper.h"

namespace utils {
namespace crypto {


static size_t get_hash(const std::vector<uint8_t>& sha256) {
	size_t hash = 0;
	for (const uint8_t& byte: sha256) {
		hash = (hash * 31) ^ byte;
	}
	return hash;
}

template <class TBuffer>
static void calculate_sha256(
		const TBuffer& data,
		size_t from,
		size_t length,
		std::vector<uint8_t>& out_sha256,
		size_t& out_hash
)
{
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, &data[from], length);
	SHA256_Final(&out_sha256[0], &sha256);
	out_hash = get_hash(out_sha256);
}

template <class TBuffer>
static void calculate_sha256_several_sources(
		const TBuffer& data,
		std::vector<uint8_t>& out_sha256,
		size_t& out_hash,
		std::initializer_list<FromLengthPair_t> sources
)
{
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	for (const FromLengthPair_t& src : sources) {
    	SHA256_Update(&sha256, &data[src.first], src.second);
	}
	SHA256_Final(&out_sha256[0], &sha256);
	out_hash = get_hash(out_sha256);
}

Sha256::Sha256(const std::string& hex_string/* = ""*/):
    _sha256(0),
    _hash(0)
{
	if (hex_string.size() == 2 * SHA256_DIGEST_LENGTH) {
		common::from_hex_string(hex_string, _sha256);
		_hash = get_hash(_sha256);
	} else {
		_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	}
}

Sha256::Sha256(
    const std::vector<uint8_t>& data,
    size_t from,
    size_t length
) :
    _sha256(SHA256_DIGEST_LENGTH),
    _hash(0)
{
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	calculate_sha256(data, from, length, _sha256, _hash);
}

Sha256::Sha256(
    const common::BufferView& data,
    size_t from,
    size_t length
) :
    _sha256(SHA256_DIGEST_LENGTH),
    _hash(0)
{
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	calculate_sha256(data, from, length, _sha256, _hash);
}

Sha256::Sha256(
    const std::vector<uint8_t>& data,
    size_t from
) :
    _sha256(
        data.begin() + from,
        data.begin() + from + SHA256_DIGEST_LENGTH
	)
{
	_hash = get_hash(_sha256);
}

Sha256::Sha256(
		const common::BufferView& data,
		size_t from
):
    _sha256(SHA256_DIGEST_LENGTH)
{
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	memcpy(&_sha256.at(0), &data.at(from), SHA256_DIGEST_LENGTH);
	_hash = get_hash(_sha256);
}

Sha256::Sha256(
		const common::BufferView& data
) :
    _sha256(SHA256_DIGEST_LENGTH)
{
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	memcpy(&_sha256.at(0), &data.at(0), SHA256_DIGEST_LENGTH);
	_hash = get_hash(_sha256);
}

Sha256::Sha256(
	    const common::BufferView& data,
		std::initializer_list<FromLengthPair_t> sources
) :
    _sha256(SHA256_DIGEST_LENGTH),
    _hash(0)
{
	_sha256.resize(SHA256_DIGEST_LENGTH, '\0');
	calculate_sha256_several_sources(data, _sha256, _hash, sources);
	reverse();
}

Sha256::Sha256(const std::vector<uint8_t>& sha):
		_sha256(sha.begin(), sha.begin() + SHA256_DIGEST_LENGTH)
{
	_hash = get_hash(_sha256);
}

Sha256::Sha256(const Sha256& other) {
	_sha256 = other._sha256;
	_hash = other._hash;
}

Sha256::Sha256(Sha256&& other) {
	_sha256 = std::move(other._sha256);
	_hash = other._hash;
}

const Sha256& Sha256::operator=(const Sha256& other)
{
	_sha256 = other._sha256;
	_hash = other._hash;
	return *this;
}

Sha256& Sha256::operator=(Sha256&& other)
{
	_sha256 = std::move(other._sha256);
	_hash = other._hash;
	return *this;
}

void Sha256::double_sha256()
{
	calculate_sha256(
			_sha256,
			0,
			SHA256_DIGEST_LENGTH,
			_sha256,
			_hash
	);
}

void Sha256::reverse()
{
	std::reverse(_sha256.begin(), _sha256.end());
	_hash = get_hash(_sha256);
}

void Sha256::clear()
{
	_sha256.clear();
	_hash = 0;
}


std::ostream& operator <<(std::ostream& out, const Sha256& sha)
{
	out << sha.repr();
	return out;
}

void Sha256::operator() (
		const std::vector<uint8_t>& data,
		size_t from,
		size_t length
)
{
	calculate_sha256(data, from, length, _sha256, _hash);
}

bool Sha256::operator==(const Sha256& other) const {
	return memcmp(&_sha256.at(0), &other._sha256.at(0), SHA256_DIGEST_LENGTH) == 0;
}

common::BufferView Sha256::sha256() const
{
	return std::move(common::BufferView(_sha256));
}

std::vector<uint8_t>
Sha256::reversed_sha256(void) const {
	std::vector<uint8_t> reversed(SHA256_DIGEST_LENGTH);
	std::reverse_copy(std::begin(_sha256), std::end(_sha256), std::begin(reversed));
	return std::move(reversed);
}

std::string Sha256::repr(void) const
{
	return common::concatinate("Sha256: ", hex_string());
}

std::string Sha256::hex_string(void) const
{
	return common::to_hex_string(_sha256);
}


size_t Sha256::size() const
{
	return SHA256_DIGEST_LENGTH;
}

size_t Sha256::hash() const
{
	return _hash;
}


size_t Sha256Hasher::operator ()(const Sha256& key) const
{
	return key.hash();
}

bool Sha256Equal::operator()(const Sha256& lhs, const Sha256& rhs) const
{
	return lhs == rhs;
}

Sha256Context::Sha256Context() {
	SHA256_CTX* ptr = new SHA256_CTX();
	_ctx_ptr = ptr;
	SHA256_Init(ptr);
}

Sha256Context::~Sha256Context() {
	SHA256_CTX* ptr = (SHA256_CTX*) _ctx_ptr;
	delete(ptr);
	_ctx_ptr = nullptr;
	ptr = nullptr;
}

void Sha256Context::update(
		const BufferView_t& data,
		size_t offset/* = 0*/,
		size_t length/* = 0*/
)
{
	SHA256_CTX* ptr = (SHA256_CTX*) _ctx_ptr;
	if (length == 0) {
		length = data.size() - offset;
	}
	SHA256_Update(ptr, &data[offset], length);
}

void Sha256Context::update(
		const std::vector<uint8_t>& data,
		size_t offset/* = 0*/,
		size_t length/* = 0*/
)
{
	update(BufferView_t(data), offset, length);
}

Sha256 Sha256Context::digest() {
	SHA256_CTX* ptr = (SHA256_CTX*) _ctx_ptr;
	std::vector<uint8_t> sha;
	sha.resize(SHA256_DIGEST_LENGTH, '\0');
	SHA256_Final(&sha[0], ptr);
	return std::move(Sha256(sha));
}

} // crypto
} // utils
