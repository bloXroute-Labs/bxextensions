#include <iostream>

#include "utils/common/byte_array.h"

#ifndef UTILS_CRYPTO_ENCRYPTED_MESSAGE_H_
#define UTILS_CRYPTO_ENCRYPTED_MESSAGE_H_


namespace utils {
namespace crypto {

class EncryptedMessage {
public:
  EncryptedMessage(int cipher_len);

  const std::vector<short>& cipher_text();
  const std::vector<short>& cipher();
  const std::vector<short>& nonce();

  common::ByteArray& nonce_array();
  common::ByteArray& cipher_array();
  common::ByteArray& cipher_text_array();


  void set_cipher_text(int cipher_start_idx);
  void resize(unsigned long long cipher_length);
  void reserve(unsigned long long cipher_capacity);
  void from_cipher_text(const std::vector<uint8_t>& cipher_text,
			int padding_len);
  void clear(void);

private:
  common::ByteArray _nonce;
  common::ByteArray _cipher;
  common::ByteArray _cipher_text;
};

} // crypto
} // utils


#endif /* UTILS_CRYPTO_ENCRYPTED_MESSAGE_H_ */
