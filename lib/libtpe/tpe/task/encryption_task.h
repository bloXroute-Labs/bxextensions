#include <vector>
#include <utils/crypto/encrypted_message.h>

#include "tpe/consts.h"
#include "tpe/task/task_base.h"

#ifndef TPE_ENCRYPTION_TASK_H_
#define TPE_ENCRYPTION_TASK_H_

namespace task {
class EncryptionTask : public TaskBase {
public:

  EncryptionTask(unsigned long long plain_capacity = PLAIN_TEXT_DEFAULT_BUFFER_SIZE);

  void init(const std::vector<uint8_t>& plain,
	    const std::vector<uint8_t>* key = nullptr);

  const std::vector<short>& cipher(void);
  const std::vector<short>& key(void);

protected:
  void _execute() override;

private:

  utils::crypto::EncryptedMessage _cipher;
  utils::common::ByteArray _plain;
  utils::common::ByteArray _key;

};
}

#endif // TPE_ENCRYPTION_TASK_H_
