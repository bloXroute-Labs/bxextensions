#include <iostream>

#include "utils/exception/error_base.h"

#ifndef UTILS_EXCEPTION_INDEX_ERROR_H_
#define UTILS_EXCEPTION_INDEX_ERROR_H_

namespace utils {
namespace exception {

class IndexError : public ErrorBase {
public:
  static const std::string ERROR_TYPE;

  IndexError(size_t idx, size_t size);
  IndexError();

  const char* what(void) const noexcept;

private:
  size_t _idx, _size;
};

} // exception
} // utils

#endif /* UTILS_EXCEPTION_INDEX_ERROR_H_ */
