#include <tpe/task/exception/task_not_executed.h>
#include <tpe/task/exception/task_not_completed.h>

#include <utils/crypto/encrypted_message.h>
#include <utils/crypto/encryption_helper.h>
#include <utils/exception/encryption_error.h>
#include <utils/exception/decryption_error.h>
#include <utils/exception/crypto_initialization_error.h>
#include <utils/exception/invalid_key_error.h>
#include <utils/exception/aggregated_exception.h>

#include "src/errors.h"

typedef task::exception::TaskNotExecuted TaskNotExecuted_t;
typedef task::exception::TaskNotCompleted TaskNotCompleted_t;
typedef utils::exception::EncryptionError EncryptionError_t;
typedef utils::exception::DecryptionError DecryptionError_t;
typedef utils::exception::InvalidKeyError InvalidKeyError_t;
typedef utils::exception::CryptoInitializationError CryptoInitializationError_t;
typedef utils::exception::AggregatedException AggregatedException_t;


/**
 * custom exception registration section:
 * template - py::registe_exception<TException>(m, "[Python error class name]"
 */
void register_errors(py::module& m) {
  py::register_exception<TaskNotExecuted_t>(m, TaskNotExecuted_t::ERROR_TYPE.c_str());
  py::register_exception<TaskNotCompleted_t>(m, TaskNotCompleted_t::ERROR_TYPE.c_str());
  py::register_exception<EncryptionError_t>(m, EncryptionError_t::ERROR_TYPE.c_str());
  py::register_exception<DecryptionError_t>(m, DecryptionError_t::ERROR_TYPE.c_str());
  py::register_exception<InvalidKeyError_t>(m, InvalidKeyError_t::ERROR_TYPE.c_str());
  py::register_exception<CryptoInitializationError_t>(
		  m,
		  CryptoInitializationError_t::ERROR_TYPE.c_str()
  );
  py::register_exception<AggregatedException_t>(
		  m,
		  AggregatedException_t::ERROR_TYPE.c_str()
  );
}
