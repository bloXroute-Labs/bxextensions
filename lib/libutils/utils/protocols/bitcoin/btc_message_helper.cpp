#include "utils/protocols/bitcoin/btc_message_helper.h"
#include "utils/protocols/bitcoin/btc_consts.h"
#include "utils/common/buffer_helper.h"
#include "utils/crypto/hash_helper.h"

namespace utils {
namespace protocols {
namespace bitcoin {
//
//static const std::unordered_map<uint8_t, std::function<size_t(
//		const std::vector<uint8_t>&,
//		unsigned long int &,
//		size_t
//		)>> varint_parsers = {
//				{0xff, get_little_endian_value<unsigned long int, unsigned long int>},
//				{0xfe, get_little_endian_value<unsigned int, unsigned long int>},
//				{0xfd, get_little_endian_value<unsigned short, unsigned long int>}
//		};
//
//unsigned long int get_var_int(
//		const std::vector<uint8_t>& buffer,
//		size_t& offset,
//		size_t type_offset) {
//
//
//}


} // bitcoin
} // protocols
} // utils
