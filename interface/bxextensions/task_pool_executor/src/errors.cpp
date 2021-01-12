#include <unordered_map>
#include <algorithm>

#include <tpe/task/exception/task_not_executed.h>
#include <tpe/task/exception/task_not_completed.h>

#include <utils/common/string_helper.h>
#include <utils/exception/encryption_error.h>
#include <utils/exception/decryption_error.h>
#include <utils/exception/crypto_initialization_error.h>
#include <utils/exception/invalid_key_error.h>
#include <utils/exception/aggregated_exception.h>
#include <utils/exception/index_error.h>
#include <utils/exception/key_error.h>

#include "src/errors.h"

typedef task::exception::TaskNotExecuted TaskNotExecuted_t;
typedef task::exception::TaskNotCompleted TaskNotCompleted_t;
typedef utils::exception::EncryptionError EncryptionError_t;
typedef utils::exception::DecryptionError DecryptionError_t;
typedef utils::exception::InvalidKeyError InvalidKeyError_t;
typedef utils::exception::IndexError IndexError_t;
typedef utils::exception::KeyError KeyError_t;
typedef utils::exception::CryptoInitializationError CryptoInitializationError_t;
typedef utils::exception::AggregatedException AggregatedException_t;
typedef utils::exception::ErrorBase ErrorBase_t;


template<class TError>
static void register_error(py::module m, std::string py_error) {
	static_assert(
			std::is_base_of<ErrorBase_t, TError>::value,
			"error template must inherit from ErrorBase"
	);
	py::register_exception<TError>(m, py_error.c_str());
#ifdef BUILD_TYPE
	if (strcmp(BUILD_TYPE, "DEBUG") || strcmp(BUILD_TYPE, "TESTING")) {
		std::string throw_name = utils::common::concatenate(
				"throw_", py_error
		);
		m.def(throw_name.c_str(), [](){throw TError();});
	}
#endif
}


/**
 * custom exception registration section:
 * template - py::register_error<TException>(m, "[Python error class name]"
 */
void register_errors(py::module& m) {
	register_error<TaskNotExecuted_t>(m, TaskNotExecuted_t::ERROR_TYPE);
	register_error<TaskNotCompleted_t>(m, TaskNotCompleted_t::ERROR_TYPE);
	register_error<EncryptionError_t>(m, EncryptionError_t::ERROR_TYPE);
	register_error<DecryptionError_t>(m, DecryptionError_t::ERROR_TYPE);
	register_error<InvalidKeyError_t>(m, InvalidKeyError_t::ERROR_TYPE);
	register_error<IndexError_t>(m, IndexError_t::ERROR_TYPE);
	register_error<KeyError_t>(m, KeyError_t::ERROR_TYPE);
	register_error<CryptoInitializationError_t>(
			m, CryptoInitializationError_t::ERROR_TYPE
	);
	register_error<AggregatedException_t>(
			m, AggregatedException_t::ERROR_TYPE

	);
}
