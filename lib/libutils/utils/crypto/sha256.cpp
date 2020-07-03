#include <algorithm>
#include <openssl/sha.h>
#include <cstring>

#include "utils/crypto/sha256.h"
#include "utils/common/string_helper.h"
#include "utils/common/buffer_helper.h"

namespace utils {
namespace crypto {

static size_t get_hash(const Sha256Binary_t& sha256) {
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
        Sha256Binary_t& out_binary,
		size_t& out_hash
)
{
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, &data[from], length);
	SHA256_Final(out_binary.data(), &sha256);
	out_hash = get_hash(out_binary);
}

template <class TBuffer>
static void calculate_sha256_several_sources(
		const TBuffer& data,
        Sha256Binary_t& out_binary,
		size_t& out_hash,
		std::initializer_list<FromLengthPair_t> sources
)
{
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	for (const FromLengthPair_t& src : sources) {
    	SHA256_Update(&sha256, &data[src.first], src.second);
	}
	SHA256_Final(out_binary.data(), &sha256);
	out_hash = get_hash(out_binary);
}

Sha256::Sha256(const std::string& hex_string/* = ""*/):
    _binary(),
    _hash(0)
{
	if (hex_string.size() == 2 * SHA256_DIGEST_LENGTH) {
	    std::vector<uint8_t> sha_vector;
		common::from_hex_string(hex_string, sha_vector);
        memcpy(_binary.data(), &sha_vector.at(0), SHA256_BINARY_SIZE);
		_hash = get_hash(_binary);
	} else {
        _binary.fill('\0');
	}
}

Sha256::Sha256(
    const std::vector<uint8_t>& data,
    size_t from,
    size_t length
) :
    _binary(),
    _hash(0)
{
	_binary.fill('\0');
	calculate_sha256(data, from, length, _binary, _hash);
}

Sha256::Sha256(
    const common::BufferView& data,
    size_t from,
    size_t length
) :
    _binary(),
    _hash(0)
{
	_binary.fill('\0');
	calculate_sha256(data, from, length, _binary, _hash);
}

Sha256::Sha256(
    const std::vector<uint8_t>& data,
    size_t from
): _binary()
{
    memcpy(_binary.data(), &data.at(from), SHA256_BINARY_SIZE);
	_hash = get_hash(_binary);
}

Sha256::Sha256(
		const common::BufferView& data,
		size_t from
): _binary()
{
	memcpy(_binary.data(), &data.at(from), SHA256_BINARY_SIZE);
	_hash = get_hash(_binary);
}

Sha256::Sha256(
		const common::BufferView& data
): _binary()
{
	memcpy(_binary.data(), &data.at(0), SHA256_BINARY_SIZE);
	_hash = get_hash(_binary);
}

Sha256::Sha256(
	    const common::BufferView& data,
		std::initializer_list<FromLengthPair_t> sources
):
    _binary(),
    _hash(0)
{
	calculate_sha256_several_sources(data, _binary, _hash, sources);
	reverse();
}

Sha256::Sha256(const std::vector<uint8_t>& sha): _binary()
{
    memcpy(_binary.data(), &sha.at(0), SHA256_BINARY_SIZE);
    _hash = get_hash(_binary);
}

Sha256::Sha256(const Sha256& other): _binary() {
	_binary = other._binary;
	_hash = other._hash;
}

Sha256::Sha256(Sha256&& other) noexcept: _binary() {
	_binary = other._binary;
	_hash = other._hash;
}

Sha256::Sha256(const Sha256Binary_t& sha): _binary(sha) {
    _hash = get_hash(_binary);
}

Sha256& Sha256::operator =(const Sha256& other)
{
	_binary = other._binary;
	_hash = other._hash;
	return *this;
}

Sha256& Sha256::operator =(Sha256&& other) noexcept
{
	_binary = other._binary;
	_hash = other._hash;
	return *this;
}

void Sha256::double_sha256()
{
	calculate_sha256(
			_binary,
			0,
			SHA256_DIGEST_LENGTH,
			_binary,
			_hash
	);
}

void Sha256::reverse()
{
	std::reverse(_binary.begin(), _binary.end());
	_hash = get_hash(_binary);
}

void Sha256::clear()
{
	_binary.fill('\0');
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
	calculate_sha256(data, from, length, _binary, _hash);
}

bool Sha256::operator==(const Sha256& other) const {
	return memcmp(_binary.data(), other._binary.data(), SHA256_DIGEST_LENGTH) == 0;
}

common::BufferView Sha256::binary() const
{
	return std::move(common::BufferView(_binary.data(), SHA256_BINARY_SIZE));
}

Sha256Binary_t
Sha256::reversed_binary() const {
    Sha256Binary_t reversed;
	std::reverse_copy(std::begin(_binary), std::end(_binary), std::begin(reversed));
	return reversed;
}

std::string Sha256::repr() const
{
	return common::concatenate("Sha256: ", hex_string());
}

std::string Sha256::hex_string() const
{
	return std::move(common::to_hex_string(_binary));
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
	auto* ptr = new SHA256_CTX();
	_ctx_ptr = ptr;
	SHA256_Init(ptr);
}

Sha256Context::~Sha256Context() {
	auto* ptr = (SHA256_CTX*) _ctx_ptr;
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
	auto* ptr = (SHA256_CTX*) _ctx_ptr;
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
	auto* ptr = (SHA256_CTX*) _ctx_ptr;
	std::vector<uint8_t> sha;
	sha.resize(SHA256_DIGEST_LENGTH, '\0');
	SHA256_Final(&sha[0], ptr);
	return std::move(Sha256(sha));
}

} // crypto
} // utils
