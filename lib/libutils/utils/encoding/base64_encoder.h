#include <string>

/*
 *  Taken from https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594
 */

#ifndef UTILS_ENCODING_BASE64_ENCODER_H_
#define UTILS_ENCODING_BASE64_ENCODER_H_

namespace utils {
namespace encoding {

static constexpr unsigned char k_decoding_table[] = {
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
        64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
        64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static constexpr char s_encoding_table[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
};


std::string base64_encode(const std::string& data) {
    size_t in_len = data.size();
    size_t out_len = 4 * ((in_len + 2) / 3);
    std::string ret(out_len, '\0');
    size_t i;
    char *p = const_cast<char *>(ret.c_str());

    for (i = 0; i < in_len - 2; i += 3) {
        *p++ = s_encoding_table[(data[i] >> 2) & 0x3F];
        *p++ = s_encoding_table[((data[i] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4)];
        *p++ = s_encoding_table[((data[i + 1] & 0xF) << 2) | ((int) (data[i + 2] & 0xC0) >> 6)];
        *p++ = s_encoding_table[data[i + 2] & 0x3F];
    }
    if (i < in_len) {
        *p++ = s_encoding_table[(data[i] >> 2) & 0x3F];
        if (i == (in_len - 1)) {
            *p++ = s_encoding_table[((data[i] & 0x3) << 4)];
            *p++ = '=';
        } else {
            *p++ = s_encoding_table[((data[i] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4)];
            *p++ = s_encoding_table[((data[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    return ret;
}

template <class TBuffer>
size_t base64_decode(const std::string &input, TBuffer& buf, size_t offset) {
    size_t in_len = input.size();
    if (in_len % 4 != 0) {
        throw std::invalid_argument("Input data size is not a multiple of 4");
    }

    size_t out_len = in_len / 4 * 3;
    size_t output_offset = offset + out_len;
    if (input[in_len - 1] == '=') out_len--;
    if (input[in_len - 2] == '=') out_len--;

    buf.resize(output_offset);

    for (size_t i = 0, j = offset; i < in_len;) {
        uint32_t a = input[i] == '=' ? 0 & i++ : k_decoding_table[static_cast<int>(input[i++])];
        uint32_t b = input[i] == '=' ? 0 & i++ : k_decoding_table[static_cast<int>(input[i++])];
        uint32_t c = input[i] == '=' ? 0 & i++ : k_decoding_table[static_cast<int>(input[i++])];
        uint32_t d = input[i] == '=' ? 0 & i++ : k_decoding_table[static_cast<int>(input[i++])];

        uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

        if (j < out_len) buf[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < out_len) buf[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < out_len) buf[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return output_offset;
}

} // encoding
} // utils

#endif //UTILS_ENCODING_BASE64_ENCODER_H_
