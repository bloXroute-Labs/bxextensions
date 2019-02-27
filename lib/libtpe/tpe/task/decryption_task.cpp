#include <utils/crypto/encryption_helper.h>

#include "tpe/task/decryption_task.h"

namespace task {

DecryptionTask::DecryptionTask(
	size_t plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/):
    MainTaskBase(),
    _cipher(utils::crypto::get_cipher_length(
	plain_capacity)) {
  _plain.reserve(plain_capacity);
}

void DecryptionTask::init(
    const std::vector<uint8_t>& cipher_text,
    const std::vector<uint8_t>& key) {
  int padding_len = utils::crypto::get_cipher_padding_length();
  _cipher.from_cipher_text(cipher_text, padding_len);
  _key.from_array(key);
  _plain.resize(_cipher.cipher_array().length());
  _plain.clear();
}

const utils::common::ByteArray& DecryptionTask::plain() {
  _assert_execution();
  return _plain;
}


void DecryptionTask::_execute(SubPool_t& sub_pool) {
  utils::crypto::decrypt(_cipher, _key, _plain);
}

}



