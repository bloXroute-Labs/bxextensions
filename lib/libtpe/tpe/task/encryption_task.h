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

  void init(const EncryptionInputBuffer_t* plain,
	    const EncryptionInputBuffer_t* key = nullptr);

  const utils::common::ByteArray& cipher(void);
  const utils::common::ByteArray & key(void);

protected:
  void _execute(SubPool_t& sub_pool) override;

private:

  utils::crypto::EncryptedMessage _cipher;
  const EncryptionInputBuffer_t* _plain;
  utils::common::ByteArray _key;

};
}

#endif // TPE_TASK_ENCRYPTION_TASK_H_
