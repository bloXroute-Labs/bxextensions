#include <memory>

extern "C" {
#include <sodium.h>
#include <sodium/crypto_secretbox.h>
}
#include <cerrno>

#include "utils/crypto/encryption_helper.h"

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
      key.set_output();
  } else if (key.length() != crypto_secretbox_KEYBYTES) {
      throw exception::InvalidKeyError(key.char_array(),
				       crypto_secretbox_KEYBYTES);
  }
}

void encrypt(
		common::ByteArray& plain,
	    common::ByteArray& key,
	    EncryptedMessage& cipher
)
{
	if (cipher.cipher_array().length() < crypto_secretbox_BOXZEROBYTES) {
	  throw std::runtime_error("cipher size is too small");
	}
	generate_key_array(key);
	randombytes_buf(cipher.nonce_array().byte_array(),
		  cipher.nonce_array().length());
	int ret = crypto_secretbox(
	  cipher.cipher_array().byte_array(),
	  plain.byte_array(),
	  plain.size(),
	  cipher.nonce_array().byte_array(),
	  key.byte_array()
	);
	if (ret < 0) {
		throw exception::EncryptionError();
	}
	cipher.set_cipher_text(crypto_secretbox_BOXZEROBYTES);
}

void decrypt(EncryptedMessage& cipher,
		 const common::BufferView& key,
	     common::ByteArray& plain_text) {
  int ret = crypto_secretbox_open(
      plain_text.byte_array(),
      cipher.cipher_array().byte_array(),
      cipher.cipher_array().length(),
      cipher.nonce_array().byte_array(),
      key.byte_array()
    );
  if (ret < 0) {
      throw exception::DecryptionError();
  }
  plain_text.shift_left(crypto_secretbox_ZEROBYTES);
  plain_text.set_output();
}

unsigned long long get_cipher_length(unsigned long long plain_length) {
  return plain_length + crypto_secretbox_ZEROBYTES;
}

int get_key_length() {
  return crypto_secretbox_KEYBYTES;
}

unsigned long long get_plain_length(unsigned long long cipher_text_length) {
  return cipher_text_length + crypto_secretbox_BOXZEROBYTES;
}

int get_padding_length() {
  return crypto_secretbox_ZEROBYTES;
}

int get_cipher_padding_length() {
  return crypto_secretbox_BOXZEROBYTES;
}

} // crypto
} // utils



