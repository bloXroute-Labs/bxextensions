#include "tpe/consts.h"
#include "tpe/task/task_base.h"
#include "utils/crypto/encrypted_message.h"

#ifndef SRC_ENCRYPTION_TASK_H_
#define SRC_ENCRYPTION_TASK_H_

namespace task {
class EncryptionTask : public TaskBase {
public:

  EncryptionTask(unsigned long long plain_capacity = PLAIN_TEXT_DEFAULT_BUFFER_SIZE);

  void init(const char *plain,
		  size_t plain_length,
		  const char *key = nullptr,
		  size_t key_length = 0);

  const utils::crypto::EncryptedMessage& cipher(void);
  const std::string& key(void);

protected:
  void _execute() override;

private:

  utils::crypto::EncryptedMessage _cipher;
  utils::common::ByteArray _plain;
  utils::common::ByteArray _key;

};
}

#endif // SRC_ENCRYPTION_TASK_H_
