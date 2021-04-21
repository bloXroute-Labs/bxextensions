#include <vector>
#include <utils/common/buffer_view.h>
#include <utils/common/tx_status_consts.h>

#ifndef UTILS_ENCODING_RLP_H_
#define UTILS_ENCODING_RLP_H_

namespace utils {
namespace encoding {

typedef common::BufferView BufferView_t;
class Rlp;
typedef std::vector<Rlp> RlpList;

class Rlp {
public:
    Rlp();
    Rlp(const BufferView_t& rlp);
    Rlp(const BufferView_t& rlp, uint64_t length, size_t val_offset = 0, size_t rlp_starting_offset = 0);
    Rlp(BufferView_t&& rlp);
    Rlp(BufferView_t&& rlp, uint64_t length, size_t val_offset = 0, size_t rlp_starting_offset = 0);
    Rlp(Rlp&& rhs);
    Rlp(const Rlp& rhs);

    Rlp& operator =(const Rlp& rhs);
    Rlp& operator =(Rlp&& rhs);

    RlpList get_rlp_list();

    BufferView_t as_rlp_string() const;

    const BufferView_t& payload() const;

    uint64_t as_int() const;
    std::vector<uint64_t> as_large_int() const;

    std::vector<uint8_t> as_vector() const;

    uint64_t length() const;
    size_t val_offset() const;
    size_t rlp_starting_offset() const;

private:

    BufferView_t _rlp;
    size_t _val_offset;
    size_t _rlp_starting_offset;
    uint64_t _length;
    uint8_t _rlp_type;
};

} // encoding
} // utils

#endif //UTILS_ENCODING_RLP_H_
