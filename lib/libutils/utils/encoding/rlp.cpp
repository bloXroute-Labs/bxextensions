#include "utils/encoding/rlp.h"
#include "utils/encoding/rlp_encoder.h"

namespace utils {
namespace encoding {

//public:
Rlp::Rlp(): _val_offset(0), _rlp_starting_offset(0), _length(0), _rlp_type (0) {
}

Rlp::Rlp(const BufferView_t& rlp): _rlp(rlp), _rlp_starting_offset(0) {
    _val_offset = consume_length_prefix(_rlp, _length, _rlp_type, _rlp_starting_offset);
}

Rlp::Rlp(const BufferView_t& rlp, size_t rlp_starting_offset): _rlp(rlp), _rlp_starting_offset(rlp_starting_offset) {
    _val_offset = consume_length_prefix(_rlp, _length, _rlp_type, _rlp_starting_offset);
}

Rlp::Rlp(
    const BufferView_t& rlp,
    uint64_t length,
    size_t val_offset/* = 0*/,
    size_t rlp_starting_offset/* = 0*/,
    uint8_t rlp_type):

    _rlp(rlp),
    _val_offset(val_offset),
    _rlp_starting_offset(rlp_starting_offset),
    _length(length),
    _rlp_type (rlp_type)
{
}

Rlp::Rlp(BufferView_t&& rlp): _rlp(std::move(rlp)), _rlp_starting_offset(0) {
    _val_offset = consume_length_prefix(_rlp, _length, _rlp_type, _rlp_starting_offset);
}

Rlp::Rlp(BufferView_t&& rlp, size_t rlp_starting_offset): _rlp(std::move(rlp)), _rlp_starting_offset(rlp_starting_offset) {
    _val_offset = consume_length_prefix(_rlp, _length, _rlp_type, _rlp_starting_offset);
}

Rlp::Rlp(
    BufferView_t&& rlp,
    uint64_t length,
    size_t val_offset/* = 0*/,
    size_t rlp_starting_offset/* = 0*/,
    uint8_t rlp_type/* = RLP_STRING*/):

    _rlp(std::move(rlp)),
    _val_offset(val_offset),
    _rlp_starting_offset(rlp_starting_offset),
    _length(length),
    _rlp_type (rlp_type)
{
}

Rlp::Rlp(Rlp&& rhs):
    _rlp(std::move(rhs._rlp)),
    _val_offset(rhs._val_offset),
    _rlp_starting_offset(rhs._rlp_starting_offset),
    _length(rhs._length),
    _rlp_type(rhs._rlp_type)
{
}

Rlp::Rlp(const Rlp& rhs):
    _rlp(rhs._rlp),
    _val_offset(rhs._val_offset),
    _rlp_starting_offset(rhs._rlp_starting_offset),
    _length(rhs._length),
    _rlp_type(rhs._rlp_type)
{
}

Rlp& Rlp::operator =(const Rlp& rhs) {
    _rlp = rhs._rlp;
    _val_offset = rhs._val_offset;
    _rlp_starting_offset = rhs._rlp_starting_offset;
    _length = rhs._length;
    _rlp_type = rhs._rlp_type;
    return *this;
}

Rlp& Rlp::operator =(Rlp&& rhs) {
    _rlp = std::move(rhs._rlp);
    _val_offset = rhs._val_offset;
    _rlp_starting_offset = rhs._rlp_starting_offset;
    _length = rhs._length;
    _rlp_type = rhs._rlp_type;
    return *this;
}

RlpList Rlp::get_rlp_list() {
    RlpList rlp_list;
    size_t offset;
    size_t end_offset;

    if ( _length == 0 ) {
        _val_offset = consume_length_prefix(_rlp, _length, _rlp_type, _rlp_starting_offset);
    }
    offset = _val_offset;
    end_offset = _val_offset + _length;

    uint64_t length;
    uint8_t rlp_type;
    while (offset < end_offset) {
        size_t next_offset = consume_length_prefix(_rlp, length, rlp_type, offset);
        size_t relative_offset = next_offset - offset;
        rlp_list.emplace_back(Rlp(_rlp, length, relative_offset, offset, rlp_type));
        offset = next_offset + length;
    }

    return rlp_list;
}

BufferView_t Rlp::as_rlp_string() const {
    if (_rlp_starting_offset + _val_offset + _length > _rlp.size()) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    if (_length > 0) {
        return BufferView_t(_rlp, _length, _val_offset + _rlp_starting_offset);
    } else {
        return BufferView_t::empty();
    }
}

const BufferView_t& Rlp::payload() const {
    return _rlp;
}

uint64_t Rlp::as_int() const {
    if (_rlp_starting_offset + _val_offset + _length > _rlp.size()) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    uint64_t val;
    size_t end_offset;
    if ( _val_offset > 0) {
        if (_length > sizeof(uint64_t) ) {
            std::vector<uint64_t> values;
            end_offset = get_big_endian_rlp_large_value(_rlp, val, _rlp_starting_offset + _val_offset, _length, values);
        } else {
            end_offset = get_big_endian_rlp_value(_rlp, val, _rlp_starting_offset + _val_offset, _length);
        }
    } else {
        end_offset = decode_int(_rlp, val, _rlp_starting_offset);
    }

    if ( end_offset > _rlp_starting_offset + _val_offset + _length ) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    return val;
}

std::vector<uint64_t> Rlp::as_large_int() const {
    if (_rlp_starting_offset + _val_offset + _length > _rlp.size()) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    std::vector<uint64_t> values;
    uint64_t val;
    size_t end_offset;
    end_offset = get_big_endian_rlp_large_value(
        _rlp, val, _rlp_starting_offset + _val_offset, _length, values
    );
    if ( end_offset == 0 or end_offset > _rlp_starting_offset + _val_offset + _length ) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    return values;
}

std::vector<uint8_t> Rlp::as_vector() const {
    if (_rlp_starting_offset + _val_offset + _length > _rlp.size()) {
        throw std::runtime_error("bad RLP!"); // TODO: throw proper exception here.
    }
    return _rlp.vector(_rlp_starting_offset + _val_offset, _length);
}

uint64_t Rlp::length() const {
    return _length;
}

size_t Rlp::val_offset() const {
    return _val_offset;
}

size_t Rlp::rlp_starting_offset() const {
    return _rlp_starting_offset;
}

uint8_t Rlp::rlp_type() const {
    return _rlp_type;
}


} // encoding
} // utils
