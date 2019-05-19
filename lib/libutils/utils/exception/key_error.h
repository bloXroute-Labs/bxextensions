#include <iostream>

#include "utils/exception/error_base.h"

#ifndef UTILS_EXCEPTION_KEY_ERROR_H_
#define UTILS_EXCEPTION_KEY_ERROR_H_

namespace utils {
namespace exception {

class KeyError : public ErrorBase {
public:
  static const std::string ERROR_TYPE;

  KeyError(const std::string& _key);
  KeyError();

  const char* what(void) const noexcept;

  const std::string& key() const;

private:
  std::string _key;
};

} // exception
} // utils

#endif /* UTILS_EXCEPTION_KEY_ERROR_H_ */
