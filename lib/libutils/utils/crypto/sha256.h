#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <initializer_list>

#include "utils/common/buffer_view.h"
#include "utils/common/default_map.h"
#include "utils/concurrency/safe_bucket_container.h"

#ifndef UTILS_CRYPTO_SHA256_H_
#define UTILS_CRYPTO_SHA256_H_

namespace utils {
namespace crypto {

typedef std::pair<size_t, size_t> FromLengthPair_t;
typedef common::BufferView BufferView_t;

class Sha256 {
public:
	Sha256(const std::string& hex_string = "");
	Sha256(
	    const std::vector<uint8_t>& data,
		size_t from,
		size_t length
	);
	Sha256(
	    const common::BufferView& data,
		size_t from,
		size_t length
	);
	Sha256(
	    const std::vector<uint8_t>& data,
		size_t from
	);
	Sha256(
	    const common::BufferView& data,
		size_t from
	);

	Sha256(const common::BufferView& data);
	Sha256(
	    const common::BufferView& data,
		std::initializer_list<FromLengthPair_t> extra_sources
	);
	Sha256(const std::vector<uint8_t>& sha);
	Sha256(const Sha256& other);
	Sha256(Sha256&& other);

	const Sha256& operator=(const Sha256& other);
	Sha256& operator=(Sha256&& other);
	friend std::ostream& operator <<(std::ostream& out, const Sha256& sha);
	bool operator==(const Sha256& other) const;
	void operator()(
			const std::vector<uint8_t>& data,
			size_t from,
			size_t length);

	void double_sha256(void);
	void reverse(void);
	void clear(void);

	common::BufferView sha256(void) const;
	std::vector<uint8_t> reversed_sha256(void) const;

	size_t size(void) const;
	size_t hash(void) const;

	std::string repr(void) const;
	std::string hex_string(void) const;

private:
	std::vector<uint8_t> _sha256;
	size_t _hash;
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



template <
	typename T
>
using Sha256DefaultMap_t = common::DefaultMap<
		Sha256,
		T,
		Sha256Hasher,
		Sha256Equal>;
typedef concurrency::SafeBucketContainer<Sha256, Sha256Hasher, Sha256Equal> Sha256BucketContainer_t;
typedef common::Bucket<Sha256, Sha256Hasher, Sha256Equal> Sha256Bucket_t;

class Sha256Context {
public:
	Sha256Context();
	~Sha256Context();

	void update(
			const BufferView_t& data,
			size_t offset = 0,
			size_t length = 0
	);

	void update(
			const std::vector<uint8_t>& data,
			size_t offset = 0,
			size_t length = 0
	);

	Sha256 digest(void);

private:
	void *_ctx_ptr;
};

} // crypto
} // utils

#endif /* UTILS_CRYPTO_SHA256_H_ */
