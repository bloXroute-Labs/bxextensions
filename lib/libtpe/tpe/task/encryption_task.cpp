#include "tpe/task/encryption_task.h"

#include "utils/crypto/encryption_helper.h"


namespace task {

EncryptionTask::EncryptionTask(
  const std::string& plain,
  const std::string& key /* = ""*/
):
  TaskBase(),
  _plain(plain),
  _key(key),
  _cipher(utils::crypto::get_cipher_length(plain.length()))
{
  _key.reserve(utils::crypto::get_key_length());
}

EncryptionTask::EncryptionTask(
    unsigned long long plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/):
    TaskBase(), _cipher(0) {
  int padding_len = utils::crypto::get_padding_length();
  _cipher.reserve(utils::crypto::get_cipher_length(
      plain_capacity + padding_len));
  _plain.reserve(plain_capacity + padding_len);
  _key.reserve(utils::crypto::get_key_length());
}

void EncryptionTask::init(const std::string& plain,
	  const std::string& key/*= ""*/) {
//  _plain.from_str(plain);
  int pad_len = utils::crypto::get_padding_length();
  _plain.resize(pad_len + plain.length());
  _plain.clear();
  strcpy(&_plain.char_array()[pad_len], plain.c_str());
  _key.from_str(key);
  _cipher.resize(utils::crypto::get_cipher_length(
      plain.length()));
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
