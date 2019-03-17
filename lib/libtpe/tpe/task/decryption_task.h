#include <iostream>

#include <utils/crypto/encrypted_message.h>

#include "tpe/consts.h"
#include "tpe/task/main_task_base.h"

#ifndef TPE_TASK_DECRYPTION_TASK_H_
#define TPE_TASK_DECRYPTION_TASK_H_

typedef utils::common::BufferView DecryptionInputBuffer_t;

namespace task {
class DecryptionTask : public MainTaskBase {
public:

  DecryptionTask(
	size_t plain_capacity =
			PLAIN_TEXT_DEFAULT_BUFFER_SIZE
  );

  void init(const DecryptionInputBuffer_t* cipher_text,
	    const DecryptionInputBuffer_t* key);

  const utils::common::ByteArray& plain(void);


protected:
  void _execute(SubPool_t& sub_pool) override;

private:

  utils::crypto::EncryptedMessage _cipher;
  utils::common::ByteArray _plain;
  const DecryptionInputBuffer_t* _key;

};
}



#endif /* TPE_TASK_DECRYPTION_TASK_H_ */
