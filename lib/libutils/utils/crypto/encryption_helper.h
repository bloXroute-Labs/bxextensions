#include <iostream>


#include "encrypted_message.h"

#ifndef UTILS_CRYPTO_ENCRYPTION_HELPER_H_
#define UTILS_CRYPTO_ENCRYPTION_HELPER_H_

namespace utils {
namespace crypto {

void init(void);

void encrypt(common::ByteArray& plain,
	     common::ByteArray& key,
	     EncryptedMessage& cipher);

void decrypt(EncryptedMessage& cipher,
	     const std::string& key,
	     common::ByteArray& plain_text);

int get_cipher_length(int plain_length);
int get_plain_length(int cipher_text_length);
int get_key_length(void);
int get_padding_length(void);

} // crypto
} // utils

#endif /* UTILS_CRYPTO_ENCRYPTION_HELPER_H_ */
