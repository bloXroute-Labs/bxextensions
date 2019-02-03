#include <iostream>

#include <utils/crypto/encrypted_message.h>

#include "tpe/consts.h"
#include "tpe/task/main_task_base.h"

#ifndef TPE_TASK_DECRYPTION_TASK_H_
#define TPE_TASK_DECRYPTION_TASK_H_

namespace task {
class DecryptionTask : public MainTaskBase {
public:

  DecryptionTask(
      unsigned long long plain_capacity =
	  PLAIN_TEXT_DEFAULT_BUFFER_SIZE);

  void init(const std::vector<uint8_t>& cipher_text,
	    const std::vector<uint8_t>& key);

  const std::vector<short>& plain(void);


protected:
  void _execute(SubPool_t& sub_pool) override;

private:

  utils::crypto::EncryptedMessage _cipher;
  utils::common::ByteArray _plain;
  utils::common::ByteArray _key;

};
}



#endif /* TPE_TASK_DECRYPTION_TASK_H_ */
