#include "tpe/task/encryption_task.h"

#include "utils/crypto/encryption_helper.h"


namespace task {

EncryptionTask::EncryptionTask(
    unsigned long long plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/):
    TaskBase(), _cipher(0) {
  int padding_len = utils::crypto::get_padding_length();
  _cipher.reserve(utils::crypto::get_cipher_length(plain_capacity));
  _plain.reserve(plain_capacity + padding_len);
  _key.reserve(utils::crypto::get_key_length());
}

void EncryptionTask::init(utils::common::ByteArray *plain,
			  utils::common::ByteArray *key/* = nullptr*/) {
  int pad_len = utils::crypto::get_padding_length();
  _plain.from_char_array(plain->char_array(), plain->length(), pad_len);
//  _plain.resize(pad_len + plain.length());
//  _plain.clear();
//  strncpy(&_plain.char_array()[pad_len], plain.c_str(), _plain.length());
  if (key != nullptr) {
      _key.from_char_array(key->char_array(), key->length());
  } else {
      _key.reset();
  }
  _cipher.resize(_plain.length());
  _cipher.clear();
  reset();
}

const utils::crypto::EncryptedMessage& EncryptionTask::cipher() {
  _wait_if_needed();
  return _cipher;
}

const std::string& EncryptionTask::key() {
  _wait_if_needed();
  return _key.str();
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
