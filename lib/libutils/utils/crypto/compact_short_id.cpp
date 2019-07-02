#include <stdint.h>
#include "utils/common/buffer_helper.h"
#include "utils/common/string_helper.h"
#include "utils/crypto/compact_short_id.h"
#include "utils/protocols/bitcoin/btc_consts.h"

namespace utils {
namespace crypto {

// from bitcoin code
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define SIPROUND do { \
    v0 += v1; v1 = ROTL(v1, 13); v1 ^= v0; \
    v0 = ROTL(v0, 32); \
    v2 += v3; v3 = ROTL(v3, 16); v3 ^= v2; \
    v0 += v3; v3 = ROTL(v3, 21); v3 ^= v0; \
    v2 += v1; v1 = ROTL(v1, 17); v1 ^= v2; \
    v2 = ROTL(v2, 32); \
} while (0)


#define HALF_ROUND(a,b,c,d,s,t)         \
a += b; c += d;             \
b = ROTL(b, s) ^ a;           \
d = ROTL(d, t) ^ c;           \
a = ROTL(a, 32);

#define DOUBLE_ROUND(v0,v1,v2,v3)       \
HALF_ROUND(v0,v1,v2,v3,13,16);      \
HALF_ROUND(v2,v1,v0,v3,17,21);      \
HALF_ROUND(v0,v1,v2,v3,13,16);      \
HALF_ROUND(v2,v1,v0,v3,17,21);


//  from bitcoin code
static uint64_t sip_hash24(
     const SipKey_t& key,
     const Sha256& sha)

{
    uint64_t k0 = key.first;
	uint64_t k1 = key.second;

    uint64_t d0, d1, d2, d3;

    std::vector<uint8_t> reversed_sha256 = std::move(sha.reversed_sha256());
    size_t offset = common::get_little_endian_value<uint64_t>(reversed_sha256, d0, 0);
    offset = common::get_little_endian_value<uint64_t>(reversed_sha256, d1, offset);
    offset = common::get_little_endian_value<uint64_t>(reversed_sha256, d2, offset);
    common::get_little_endian_value<uint64_t>(reversed_sha256, d3, offset);

    uint64_t v0 = 0x736f6d6570736575ULL ^ k0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ k1;
    uint64_t v2 = 0x6c7967656e657261ULL ^ k0;
    uint64_t v3 = 0x7465646279746573ULL ^ k1 ^ d0;

    SIPROUND;
    SIPROUND;
    v0 ^= d0;
//     d = val.GetUint64(1);
    v3 ^= d1;
    SIPROUND;
    SIPROUND;
    v0 ^= d1;
//    d = val.GetUint64(2);
    v3 ^= d2;
    SIPROUND;
    SIPROUND;
    v0 ^= d2;
//    d = val.GetUint64(3);
    v3 ^= d3;
    SIPROUND;
    SIPROUND;
    v0 ^= d3;

    v3 ^= ((uint64_t)4) << 59;
    SIPROUND;
    SIPROUND;
    v0 ^= ((uint64_t)4) << 59;
    v2 ^= 0xFF;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;

   return (v0 ^ v1) ^ (v2 ^ v3);
}


CompactShortId::CompactShortId(
	    const common::BufferView& data,
		size_t offset
)
{
    common::get_compact_short_id(data, _compact_short_id, offset);
}

CompactShortId::CompactShortId(
	    const SipKey_t& key,
	    const Sha256& data
)
{
    _compact_short_id = sip_hash24(key, data);
    _compact_short_id &= 0x0000FFFFFFFFFFFFL;
}

CompactShortId::CompactShortId(CompactShortId&& src):
		_compact_short_id(std::move(src._compact_short_id))
{

}
CompactShortId::CompactShortId(const CompactShortId& src):
		_compact_short_id(src._compact_short_id)
{

}

CompactShortId& CompactShortId::operator =(CompactShortId&& src) {
	_compact_short_id = std::move(src._compact_short_id);
	return *this;
}

CompactShortId& CompactShortId::operator =(const CompactShortId& src) {
	_compact_short_id = src._compact_short_id;
	return *this;
}

uint64_t CompactShortId::compact_short_id() const {
	return _compact_short_id;
}

size_t CompactShortId::hash(void) const {
	return _compact_short_id;
}

const size_t CompactShortId::size = BTC_COMPACT_SHORT_ID_SIZE;

size_t CompactShortIdHasher::operator() (
    const CompactShortId& compact_short_id
) const
{
	return compact_short_id.hash();
}

bool CompactShortIdEqual::operator() (
    const CompactShortId& lhs,
    const CompactShortId& rhs
) const
{
	return lhs.compact_short_id() == rhs.compact_short_id();
}

} // crypto
} // utils
