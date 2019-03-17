#include <cstring>

extern "C" {
#include <sodium/crypto_secretbox.h>
}

#include "encrypted_message.h"

#define NONCE_LEN crypto_secretbox_NONCEBYTES

namespace utils {
namespace crypto {

EncryptedMessage::EncryptedMessage(size_t cipher_len):
  _nonce(NONCE_LEN),
  _cipher(cipher_len),
  _cipher_text(NONCE_LEN + cipher_len) {
}

const common::ByteArray& EncryptedMessage::cipher_text() {
  return _cipher_text;
}

const common::ByteArray& EncryptedMessage::cipher() {
  return _cipher;
}

const common::ByteArray& EncryptedMessage::nonce() {
  return _nonce;
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

void EncryptedMessage::set_cipher_text(int cipher_start_idx) {
  _cipher.shift_left(cipher_start_idx);
  _cipher_text.resize(_cipher.length() + NONCE_LEN);
  _cipher_text.clear();
  memcpy(_cipher_text.char_array(),
	    _nonce.char_array(),
	    NONCE_LEN);
  memcpy(&_cipher_text.char_array()[NONCE_LEN],
	    _cipher.char_array(),
	    _cipher.length());
  _cipher.set_output();
  _cipher_text.set_output();
}

void EncryptedMessage::from_cipher_text(
    const std::vector<uint8_t>& cipher_text,
    int padding_len) {
  _cipher.resize(cipher_text.size() + padding_len - NONCE_LEN);
  _cipher.clear();
  _cipher_text.from_array(cipher_text);
  memcpy(_nonce.char_array(),
	  _cipher_text.char_array(),
	  NONCE_LEN);
  memcpy(&_cipher.char_array()[padding_len],
	  &_cipher_text.char_array()[NONCE_LEN],
	  _cipher.length() -  padding_len);
}

void EncryptedMessage::resize(size_t cipher_length) {
  _cipher.resize(cipher_length);
  _cipher_text.resize(cipher_length + NONCE_LEN);
}


void EncryptedMessage::reserve(size_t cipher_capacity) {
  _cipher.reserve(cipher_capacity);
  _cipher_text.reserve(cipher_capacity + NONCE_LEN);
}


void EncryptedMessage::clear() {
  _cipher.clear();
  _cipher_text.clear();
  _nonce.clear();
}

} // crypto
} // utils


