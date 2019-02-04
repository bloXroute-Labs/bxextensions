#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>

#ifndef UTILS_CRYPTO_SHA256_H_
#define UTILS_CRYPTO_SHA256_H_

namespace utils {
namespace crypto {

class Sha256 {
public:
	Sha256();
	Sha256(const std::vector<uint8_t>& data,
			size_t from,
			size_t length);
	Sha256(const Sha256& other) = delete;
	Sha256(Sha256&& other);

	const Sha256& operator=(const Sha256& other) = delete;
	const Sha256& operator++();
	bool operator==(const Sha256& other) const;
	void operator()(
			const std::vector<uint8_t>& data,
			size_t from,
			size_t length);

	const std::vector<uint8_t>& sha256(void);

	size_t size(void) const;

	size_t hash(void) const;

private:
	std::vector<uint8_t> _sha256;
};

class Sha256Hasher {
public:
	size_t operator()(const Sha256& key) const;
};

class Sha256Equal {
public:
	bool operator()(const Sha256& lhs, const Sha256& rhs) const;
};

template <typename T>
using Sha256Map_t = std::unordered_map<
		Sha256,
		T,
		Sha256Hasher,
		Sha256Equal>;

} // crypto
} // utils

#endif /* UTILS_CRYPTO_SHA256_H_ */
