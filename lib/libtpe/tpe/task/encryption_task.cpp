#include <utils/crypto/encryption_helper.h>

#include "tpe/task/encryption_task.h"


namespace task {

EncryptionTask::EncryptionTask(
    unsigned long long plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/):
    TaskBase(), _cipher(0) {
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
  reset();
}

const std::vector<short>& EncryptionTask::cipher() {
  _wait_if_needed();
  return _cipher.cipher_text();
}

const std::vector<short>& EncryptionTask::key() {
  _wait_if_needed();
  return _key.array();
}

void EncryptionTask::_execute() {
//  unsigned long long t1 = std::chrono::duration_cast<std::chrono::milliseconds>(
//        std::chrono::system_clock::now().time_since_epoch()).count();
//  std::cout<< get_id() << " started encrypting at: " << t1 << std::endl;
  utils::crypto::encrypt(_plain, _key, _cipher);
//  unsigned long long t2 = std::chrono::duration_cast<std::chrono::milliseconds>(
//        std::chrono::system_clock::now().time_since_epoch()).count();
//  std::cout<< get_id() << " encryption end time (ms): " << t2 << std::endl;
}

}
