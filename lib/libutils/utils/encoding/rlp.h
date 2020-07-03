#include <vector>
#include "utils/common/buffer_view.h"

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
    Rlp(const BufferView_t& rlp, uint64_t length, size_t offset = 0);
    Rlp(BufferView_t&& rlp);
    Rlp(BufferView_t&& rlp, uint64_t length, size_t offset = 0);
    Rlp(Rlp&& rhs);
    Rlp(const Rlp& rhs);

    Rlp& operator =(const Rlp& rhs);
    Rlp& operator =(Rlp&& rhs);

    Rlp get_next_rlp_string();
    RlpList get_next_rlp_list();

    BufferView_t as_rlp_string() const;

    const BufferView_t& payload() const;

    uint64_t as_int() const;

    size_t length() const;
    size_t offset() const;

private:

    BufferView_t _rlp;
    size_t _offset;
    uint64_t _length;
};

} // encoding
} // utils

#endif //UTILS_ENCODING_RLP_H_
