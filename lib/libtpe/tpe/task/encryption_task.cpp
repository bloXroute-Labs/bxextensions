#include <utils/crypto/encryption_helper.h>

#include "tpe/task/encryption_task.h"


namespace task {

EncryptionTask::EncryptionTask(
	size_t plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/
) : MainTaskBase(), _cipher(0)
{
	int padding_len = utils::crypto::get_padding_length();
	_cipher.reserve(utils::crypto::get_cipher_length(plain_capacity));
	_plain.reserve(plain_capacity + padding_len);
	_key.reserve(utils::crypto::get_key_length());
}

void EncryptionTask::init(
		EncryptionInputBuffer_t plain,
	    EncryptionInputBuffer_t key/* = EncryptionInputBuffer_t::empty()*/
)
{
	int pad_len = utils::crypto::get_padding_length();
	_plain.copy_from_buffer(plain, pad_len, 0, plain.size());
	if (key) {
	  _key.copy_from_buffer(key, 0, 0, key.size());
	} else {
	  _key.reset();
	}
	_cipher.resize(_plain.size());
	_cipher.clear();
}

const utils::common::ByteArray& EncryptionTask::cipher() {
	assert_execution();
	return _cipher.cipher_text();
}

const utils::common::ByteArray & EncryptionTask::key() {
	assert_execution();
	return _key;
}

void EncryptionTask::_execute(SubPool_t& sub_pool) {
	utils::crypto::encrypt(_plain, _key, _cipher);
}

}
