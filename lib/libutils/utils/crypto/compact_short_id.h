#include <iostream>
#include <vector>
#include <cstdint>

#include "utils/common/buffer_view.h"
#include "utils/crypto/sha256.h"

#ifndef UTILS_CRYPTO_COMPACTSHORTID_H_
#define UTILS_CRYPTO_COMPACTSHORTID_H_

namespace utils {
namespace crypto {

typedef std::pair<uint64_t, uint64_t> SipKey_t;

class CompactShortId
{
public:

	CompactShortId(
	    const common::BufferView& data,
		size_t offset
	);

	CompactShortId(
	    const SipKey_t& key,
	    const Sha256& data
	);

	CompactShortId(CompactShortId&& src);
	CompactShortId(const CompactShortId& src);

	CompactShortId& operator =(CompactShortId&& src);
	CompactShortId& operator =(const CompactShortId& src);

	size_t hash(void) const;
	uint64_t compact_short_id() const;
	static const size_t size;

private:
    uint64_t _compact_short_id;
};

class CompactShortIdHasher {
public:
	size_t operator()(const CompactShortId& key) const;
};

class CompactShortIdEqual {
public:
	bool operator()(const CompactShortId& lhs, const CompactShortId& rhs) const;
};

template <typename T>
using CompactShortIdMap_t = std::unordered_map<
		CompactShortId,
		T,
		CompactShortIdHasher,
		CompactShortIdEqual>;


} // crypto
} // utils

#endif // UTILS_CRYPTO_COMPACTSHORTID_H_
