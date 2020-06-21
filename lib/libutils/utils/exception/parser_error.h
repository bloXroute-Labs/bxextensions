#include "utils/exception/error_base.h"

#ifndef UTILS_EXCEPTION_PARSER_ERROR_H
#define UTILS_EXCEPTION_PARSER_ERROR_H

namespace utils {
namespace exception {

class ParserError : public ErrorBase {
public:
    static const std::string ERROR_TYPE;

    ParserError();

    const char* what(void) const noexcept;
};


}
}
#endif //UTILS_EXCEPTION_PARSER_ERROR_H
