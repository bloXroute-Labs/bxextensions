#include <iostream>

#include "tpe/consts.h"
#include "tpe/task/task_base.h"
#include "utils/crypto/encrypted_message.h"

#ifndef SRC_TASK_DECRYPTION_TASK_H_
#define SRC_TASK_DECRYPTION_TASK_H_

namespace task {
class DecryptionTask : public TaskBase {
public:

  DecryptionTask(
      unsigned long long plain_capacity =
	  PLAIN_TEXT_DEFAULT_BUFFER_SIZE);

  void init(const std::string& cipher_text,
	    const std::string& key);

  const std::string& plain(void);


protected:
  void _execute() override;

private:

  utils::crypto::EncryptedMessage _cipher;
  utils::common::ByteArray _plain;
  std::string _key;

};
}



#endif /* SRC_TASK_DECRYPTION_TASK_H_ */
