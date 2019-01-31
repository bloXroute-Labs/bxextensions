#include <tpe/task/exception/task_not_executed.h>

#include <utils/crypto/encrypted_message.h>
#include <utils/crypto/encryption_helper.h>
#include <utils/exception/encryption_error.h>
#include <utils/exception/decryption_error.h>
#include <utils/exception/crypto_initialization_error.h>
#include <utils/exception/invalid_key_error.h>

#include "src/errors.h"

typedef task::exception::TaskNotExecuted TaskNotExecuted_t;
typedef utils::exception::EncryptionError EncryptionError_t;
typedef utils::exception::DecryptionError DecryptionError_t;
typedef utils::exception::InvalidKeyError InvalidKeyError_t;
typedef utils::exception::CryptoInitializationError CryptoInitializationError_t;


/**
 * custom exception registration section:
 * template - py::registe_exception<TException>(m, "[Python error class name]"
 */
void register_errors(py::module& m) {
  py::register_exception<TaskNotExecuted_t>(m, "TaskNotExecutedError");
  py::register_exception<EncryptionError_t>(m, "EncryptionError");
  py::register_exception<DecryptionError_t>(m, "DecryptionError");
  py::register_exception<InvalidKeyError_t>(m, "InvalidKeyError");
  py::register_exception<CryptoInitializationError_t>(m, "CryptoInitializationError");
}
