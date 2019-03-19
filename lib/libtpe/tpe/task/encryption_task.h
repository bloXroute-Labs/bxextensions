#include <vector>
#include <utils/crypto/encrypted_message.h>

#include "tpe/consts.h"
#include "tpe/task/main_task_base.h"

#ifndef TPE_TASK_ENCRYPTION_TASK_H_
#define TPE_TASK_ENCRYPTION_TASK_H_

typedef utils::common::BufferView EncryptionInputBuffer_t;

namespace task {
class EncryptionTask : public MainTaskBase {
public:

  EncryptionTask(size_t plain_capacity = PLAIN_TEXT_DEFAULT_BUFFER_SIZE);

  void init(
		  EncryptionInputBuffer_t plain,
	      EncryptionInputBuffer_t key = EncryptionInputBuffer_t::empty()
  );

  const utils::common::ByteArray& cipher(void);
  const utils::common::ByteArray & key(void);

protected:
  void _execute(SubPool_t& sub_pool) override;

private:

  utils::crypto::EncryptedMessage _cipher;
  utils::common::ByteArray _plain;
  utils::common::ByteArray _key;

};
}

#endif // TPE_TASK_ENCRYPTION_TASK_H_
