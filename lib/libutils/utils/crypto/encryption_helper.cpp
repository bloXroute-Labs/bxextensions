#include <memory>
#include <chrono>

extern "C" {
#include <sodium.h>
#include <sodium/crypto_secretbox.h>
}

#include "encryption_helper.h"

#include "utils/exception/encryption_error.h"
#include "utils/exception/decryption_error.h"
#include "utils/exception/crypto_initialization_error.h"
#include "utils/exception/invalid_key_error.h"

namespace utils {
namespace crypto {

void init() {
  if (sodium_init() < 0) {
      throw exception::CryptoInitializationError();
  }
}

static void generate_key_array(
    common::ByteArray& key) {
  if (key.length() == 0) {
      key.resize(crypto_secretbox_KEYBYTES);
      key.clear();
      crypto_secretbox_keygen(key.byte_array());
  } else if (key.length() != crypto_secretbox_KEYBYTES) {
      throw exception::InvalidKeyError(key.str(),
				       crypto_secretbox_KEYBYTES);
  }
}

void encrypt(common::ByteArray& plain,
	     common::ByteArray& key,
	     EncryptedMessage& cipher) {
  generate_key_array(key);
  randombytes_buf(cipher.nonce_array().byte_array(),
		  cipher.nonce_array().length());
//  unsigned long long t1 = std::chrono::duration_cast<std::chrono::milliseconds>(
//        std::chrono::system_clock::now().time_since_epoch()).count();
  int ret = crypto_secretbox(
      cipher.cipher_array().byte_array(),
      plain.byte_array(),
      plain.length(),
      cipher.nonce_array().byte_array(),
      key.byte_array()
    );
//  unsigned long long t2 = std::chrono::duration_cast<std::chrono::milliseconds>(
//        std::chrono::system_clock::now().time_since_epoch()).count();
//  std::cout << "encryption time (ms): " << t2 - t1 << std::endl;
  if (ret < 0) {
      throw exception::EncryptionError();
  }
  cipher.set_cipher_text();
//  clock_t end = clock();
//  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//  std::cout << "elapsed time: " << elapsed_secs << std::endl;

}

void decrypt(EncryptedMessage& cipher,
	     const std::string& key,
	     common::ByteArray& plain_text) {
  int ret = crypto_secretbox_open_easy(
      plain_text.byte_array(),
      cipher.cipher_array().byte_array(),
      cipher.cipher_array().length(),
      cipher.nonce_array().byte_array(),
      (const unsigned char*)key.c_str()
    );
  if (ret < 0) {
      throw exception::DecryptionError();
  }
}

int get_cipher_length(int plain_length) {
  return plain_length + crypto_secretbox_MACBYTES;
}

int get_key_length() {
  return crypto_secretbox_KEYBYTES;
}

int get_plain_length(int cipher_text_length) {
  return cipher_text_length - crypto_secretbox_MACBYTES -
      crypto_secretbox_NONCEBYTES;
}

int get_padding_length() {
  return crypto_secretbox_ZEROBYTES;
}

} // crypto
} // utils



