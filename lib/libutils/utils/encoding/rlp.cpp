#include "utils/encoding/rlp.h"
#include "utils/encoding/rlp_encoder.h"

namespace utils {
namespace encoding {

//public:
Rlp::Rlp(): _offset(0), _length(0) {

}

Rlp::Rlp(const BufferView_t& rlp): _rlp(rlp), _offset(0), _length(0) {

}

Rlp::Rlp(const BufferView_t& rlp, uint64_t length, size_t offset/* = 0*/): _rlp(rlp), _offset(offset), _length(length) {

}

Rlp::Rlp(BufferView_t&& rlp): _rlp(std::move(rlp)), _offset(0), _length(0) {

}

Rlp::Rlp(BufferView_t&& rlp, uint64_t length, size_t offset/* = 0*/): _rlp(std::move(rlp)), _offset(offset), _length(length) {

}

Rlp::Rlp(Rlp&& rhs): _rlp(std::move(rhs._rlp)), _offset(rhs._offset), _length(rhs._length) {

}

Rlp::Rlp(const Rlp& rhs): _rlp(rhs._rlp), _offset(rhs._offset), _length(rhs._length) {

}

Rlp& Rlp::operator =(const Rlp& rhs) {
    _rlp = rhs._rlp;
    _offset = rhs._offset;
    _length = rhs._length;
    return *this;
}

Rlp& Rlp::operator =(Rlp&& rhs) {
    _rlp = std::move(rhs._rlp);
    _offset = rhs._offset;
    _length = rhs._length;
    return *this;
}

Rlp Rlp::get_next_rlp_string() {
    uint64_t length;
    size_t next_offset = get_length_prefix(_rlp, length, _offset);
    size_t relative_offset = next_offset - _offset;
    BufferView_t rlp_string(_rlp, relative_offset + length, _offset);
    _offset = next_offset + length;
    return std::move(Rlp(std::move(rlp_string), length, relative_offset));
}

RlpList Rlp::get_next_rlp_list() {
    RlpList rlp_list;
    uint64_t list_length;
    size_t offset = get_length_prefix(_rlp, list_length, _offset);
    _offset = offset + list_length;
    while (offset < _offset) {
        uint64_t length;
        size_t next_offset = get_length_prefix(_rlp, length, offset);
        size_t relative_offset = next_offset - offset;
        BufferView_t rlp_string(_rlp, relative_offset + length, offset);
        rlp_list.emplace_back(std::move(rlp_string), length, relative_offset);
        offset = next_offset + length;
    }
    return std::move(rlp_list);
}

BufferView_t Rlp::as_rlp_string() const {
    if (_offset + _length != _rlp.size()) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    if (_length > 0) {
        return std::move(BufferView_t(_rlp, _length, _offset));
    } else {
        return BufferView_t::empty();
    }
}

const BufferView_t& Rlp::payload() const {
    return _rlp;
}


uint64_t Rlp::as_int() const {
    if (_offset + _length != _rlp.size()) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    uint64_t val;
    if (rlp_decode(_rlp, val, 0) != _offset + _length) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    return val;
}

size_t Rlp::length() const {
    return _length;
}

size_t Rlp::offset() const {
    return _offset;
}


} // encoding
} // utils