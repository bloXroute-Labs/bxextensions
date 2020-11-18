#include <utils/crypto/encryption_helper.h>
#include <cstring>

#include "tpe/task/decryption_task.h"

namespace task {

DecryptionTask::DecryptionTask(
	size_t plain_capacity /*= PLAIN_TEXT_DEFAULT_BUFFER_SIZE*/
):
		MainTaskBase(),
		_cipher(utils::crypto::get_cipher_length(plain_capacity)),
		_key(nullptr)
{
	_plain.reserve(plain_capacity);
}

void DecryptionTask::init(
    const DecryptionInputBuffer_t* cipher_text,
    const DecryptionInputBuffer_t* key
)
{
	int padding_len = utils::crypto::get_cipher_padding_length();
	std::vector<uint8_t> ct;
	ct.resize(cipher_text->size(), '\0');
	memcpy(&ct.at(0), cipher_text->byte_array(), cipher_text->size());
	_cipher.from_cipher_text(ct, padding_len);
	_key = key;
	_plain.resize(_cipher.cipher_array().length());
	_plain.clear();
}

const utils::common::ByteArray& DecryptionTask::plain() {
	assert_execution();
	return _plain;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void DecryptionTask::_execute(SubPool_t& sub_pool) {
	utils::crypto::decrypt(_cipher, *_key, _plain);
}
#pragma GCC diagnostic pop

}



