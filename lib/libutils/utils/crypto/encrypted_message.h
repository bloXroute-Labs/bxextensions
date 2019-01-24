#include <iostream>

#include "utils/common/byte_array.h"

#ifndef UTILS_CRYPTO_ENCRYPTED_MESSAGE_H_
#define UTILS_CRYPTO_ENCRYPTED_MESSAGE_H_


namespace utils {
namespace crypto {

class EncryptedMessage {
public:
  EncryptedMessage(int cipher_len);

  EncryptedMessage(const std::string& cipher_text);

  const std::string& cipher_text();
  const std::string& cipher();
  const std::string& nonce();

  common::ByteArray& nonce_array();
  common::ByteArray& cipher_array();
  common::ByteArray& cipher_text_array();


  void set_cipher_text(void);
  void resize(unsigned long long cipher_length);
  void reserve(unsigned long long cipher_capacity);
  void from_cipher_text(const std::string& cipher_text);
  void clear(void);

private:
  common::ByteArray _nonce;
  common::ByteArray _cipher;
  common::ByteArray _cipher_text;
};

} // crypto
} // utils


#endif /* UTILS_CRYPTO_ENCRYPTED_MESSAGE_H_ */
