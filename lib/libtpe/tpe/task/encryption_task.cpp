#include <utils/crypto/encryption_helper.h>

#include "tpe/task/encryption_task.h"


namespace task {

EncryptionTask::EncryptionTask(
	size_t plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/
) : MainTaskBase(), _cipher(0), _plain(nullptr)
{
	int padding_len = utils::crypto::get_padding_length();
	_cipher.reserve(utils::crypto::get_cipher_length(plain_capacity));
	_key.reserve(utils::crypto::get_key_length());
}

void EncryptionTask::init(
		const EncryptionInputBuffer_t* plain,
	    const EncryptionInputBuffer_t* key/* = nullptr*/
)
{
	int pad_len = utils::crypto::get_padding_length();
	_plain = plain;
	if (key != nullptr) {
	  _key.copy_from_buffer(*key, 0, 0, key->size());
	} else {
	  _key.reset();
	}
	_cipher.resize(_plain->size());
	_cipher.clear();
}

const utils::common::ByteArray& EncryptionTask::cipher() {
	_assert_execution();
	return _cipher.cipher_text();
}

const utils::common::ByteArray & EncryptionTask::key() {
	_assert_execution();
	return _key;
}

void EncryptionTask::_execute(SubPool_t& sub_pool) {
	utils::crypto::encrypt(*_plain, _key, _cipher);
}

}
