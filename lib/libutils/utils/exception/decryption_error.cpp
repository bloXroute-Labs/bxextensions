#include "utils/exception/decryption_error.h"

namespace utils {
namespace exception {

DecryptionError::DecryptionError():
		ErrorBase(ERROR_TYPE)
{
}

const char* DecryptionError::what(void) const noexcept  {
    return common::concatenate(
  		  "Failed to decrypt message\n",
		  _get_backtrace()
    ).c_str();
}

const std::string DecryptionError::ERROR_TYPE = "DecryptionError";


} // exception
} // utils
