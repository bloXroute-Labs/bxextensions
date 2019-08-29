#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <initializer_list>

#include <utils/common/ordered_map.h>
#include "utils/common/buffer_view.h"
#include "utils/common/default_map.h"
#include "utils/common/map_wrapper.h"
#include "utils/concurrency/safe_bucket_container.h"
#include "utils/common/tracked_allocator.h"

#ifndef UTILS_CRYPTO_SHA256_H_
#define UTILS_CRYPTO_SHA256_H_

#define SHA256_BINARY_SIZE 32

namespace utils {
namespace crypto {

typedef std::pair<size_t, size_t> FromLengthPair_t;
typedef common::BufferView BufferView_t;
typedef std::array<uint8_t, SHA256_BINARY_SIZE> Sha256Binary_t;

class Sha256 {
public:
	explicit Sha256(const std::string& hex_string = "");
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

	explicit Sha256(const common::BufferView& data);
	Sha256(
	    const common::BufferView& data,
		std::initializer_list<FromLengthPair_t> extra_sources
	);
	explicit Sha256(const std::vector<uint8_t>& sha);
	Sha256(const Sha256& other);
	Sha256(Sha256&& other) noexcept;

	Sha256& operator =(const Sha256& other);
	Sha256& operator =(Sha256&& other) noexcept;

	friend std::ostream& operator <<(std::ostream& out, const Sha256& sha);
	bool operator==(const Sha256& other) const;
	void operator()(
			const std::vector<uint8_t>& data,
			size_t from,
			size_t length);

	void double_sha256();
	void reverse();
	void clear();

	common::BufferView binary() const;
    Sha256Binary_t reversed_binary() const;

	size_t size() const;
	size_t hash() const;

	std::string repr() const;
	std::string hex_string() const;

private:
    Sha256Binary_t _binary;
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

template <class T>
using Sha256MapAllocator_t = common::TrackedAllocator<std::pair<const Sha256, T>>;

template <class T>
using Sha256OrderedMapAllocator_t = common::TrackedAllocator<std::pair<Sha256, T>>;

typedef common::TrackedAllocator<Sha256> Sha256Allocator_t;

template <typename T>
using Sha256Map_t = std::unordered_map<
		Sha256,
		T,
		Sha256Hasher,
		Sha256Equal,
        Sha256MapAllocator_t<T>>;

template <typename T>
using Sha256MapWrapper_t = common::MapWrapper<
        Sha256,
        T,
        Sha256Hasher,
        Sha256Equal,
        Sha256MapAllocator_t<T>>;


template <typename T>
using Sha256DefaultMap_t = common::DefaultMap<
		Sha256,
		T,
		Sha256Hasher,
        Sha256Equal,
        Sha256MapAllocator_t<T>>;

template <typename T>
using Sha256OrderedMap_t = common::OrderedMap<
        Sha256,
        T,
        Sha256Hasher,
        Sha256Equal,
        Sha256OrderedMapAllocator_t<T>>;


typedef concurrency::SafeBucketContainer<Sha256, Sha256Hasher, Sha256Equal, Sha256Allocator_t> Sha256BucketContainer_t;
typedef common::Bucket<Sha256, Sha256Hasher, Sha256Equal, Sha256Allocator_t> Sha256Bucket_t;

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

	Sha256 digest();

private:
	void *_ctx_ptr;
};

} // crypto
} // utils

#endif /* UTILS_CRYPTO_SHA256_H_ */
