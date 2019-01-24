#include "tpe/task/decryption_task.h"

#include "utils/crypto/encryption_helper.h"

namespace task {

DecryptionTask::DecryptionTask(
    const std::string& cipher_text,
    const std::string& key):
	TaskBase(),
	_cipher(cipher_text),
	_key(key)
{
  _plain.resize(utils::crypto::get_plain_length(
      cipher_text.length()));
  _plain.clear();
}

DecryptionTask::DecryptionTask(
    unsigned long long plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/):
    TaskBase(),
    _cipher(utils::crypto::get_cipher_length(
	plain_capacity)) {
  _plain.reserve(plain_capacity);
}

void DecryptionTask::init(
    const std::string& cipher_text,
    const std::string& key) {
  _cipher.from_cipher_text(cipher_text);
  _key = key;
  _plain.resize(utils::crypto::get_plain_length(
      cipher_text.length()));
  _plain.clear();
  reset();
}

const std::string& DecryptionTask::plain() {
  _wait_if_needed();
  return _plain.str();
}


void DecryptionTask::_execute() {
  utils::crypto::decrypt(_cipher, _key, _plain);
}

}



