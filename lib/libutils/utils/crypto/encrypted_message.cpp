#include <cstring>

extern "C" {
#include <sodium/crypto_secretbox.h>
}

#include "encrypted_message.h"

#define NONCE_LEN crypto_secretbox_NONCEBYTES

namespace utils {
namespace crypto {

EncryptedMessage::EncryptedMessage(int cipher_len):
  _nonce(NONCE_LEN),
  _cipher(cipher_len),
  _cipher_text(NONCE_LEN + cipher_len) {
}

EncryptedMessage::EncryptedMessage(const std::string& cipher_text):
  _nonce(NONCE_LEN),
  _cipher(cipher_text.length() - NONCE_LEN),
  _cipher_text(cipher_text) {
  strncpy(_nonce.char_array(),
	  _cipher_text.char_array(),
	  _nonce.length());
  strncpy(_cipher.char_array(),
	  &_cipher_text.char_array()[_nonce.length()],
	  _cipher.length());
}

const std::string& EncryptedMessage::cipher_text() {
  return _cipher_text.str();
}

const std::string& EncryptedMessage::cipher() {
  return _cipher.str();
}

const std::string& EncryptedMessage::nonce() {
  return _nonce.str();
}

common::ByteArray& EncryptedMessage::nonce_array() {
  return _nonce;
}

common::ByteArray& EncryptedMessage::cipher_array() {
  return _cipher;
}

common::ByteArray& EncryptedMessage::cipher_text_array() {
  return _cipher_text;
}

void EncryptedMessage::set_cipher_text(void) {
  strncpy(_cipher_text.char_array(),
	    _nonce.char_array(),
	    _nonce.length());
  strncpy(&_cipher_text.char_array()[_nonce.length()],
	    _cipher.char_array(),
	    _cipher.length());
}

void EncryptedMessage::from_cipher_text(
    const std::string& cipher_text) {
  _cipher.resize(cipher_text.length() - NONCE_LEN);
  _cipher_text.from_str(cipher_text);
  strncpy(_nonce.char_array(),
	  _cipher_text.char_array(),
	  _nonce.length());
  strncpy(_cipher.char_array(),
	  &_cipher_text.char_array()[_nonce.length()],
	  _cipher.length());
}

void EncryptedMessage::resize(unsigned long long cipher_length) {
  _cipher.resize(cipher_length);
  _cipher_text.resize(cipher_length + NONCE_LEN);
}


void EncryptedMessage::reserve(unsigned long long cipher_capacity) {
  _cipher.reserve(cipher_capacity);
  _cipher_text.reserve(cipher_capacity);
}


void EncryptedMessage::clear() {
  _cipher.clear();
  _cipher_text.clear();
}

} // crypto
} // utils


