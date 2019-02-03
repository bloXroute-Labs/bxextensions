#include <utils/crypto/encryption_helper.h>

#include "tpe/task/encryption_task.h"


namespace task {

EncryptionTask::EncryptionTask(
    unsigned long long plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/):
    MainTaskBase(), _cipher(0) {
  int padding_len = utils::crypto::get_padding_length();
  _cipher.reserve(utils::crypto::get_cipher_length(plain_capacity));
  _plain.reserve(plain_capacity + padding_len);
  _key.reserve(utils::crypto::get_key_length());
}

void EncryptionTask::init(const std::vector<uint8_t>& plain,
	    const std::vector<uint8_t>* key/* = nullptr*/) {
  int pad_len = utils::crypto::get_padding_length();
  _plain.from_array(plain, pad_len);
  if (key != nullptr) {
      _key.from_array(*key);
  } else {
      _key.reset();
  }
  _cipher.resize(_plain.length());
  _cipher.clear();
}

const std::vector<short>& EncryptionTask::cipher() {
  _assert_execution();
  return _cipher.cipher_text();
}

const std::vector<short>& EncryptionTask::key() {
  _assert_execution();
  return _key.array();
}

void EncryptionTask::_execute(SubPool_t& sub_pool) {
  utils::crypto::encrypt(_plain, _key, _cipher);
}

}
