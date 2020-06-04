//
// Created by danielle machpud on 25/05/2020.
//

#include "utils/exception/parser_error.h"

namespace utils {
namespace exception {

ParserError::ParserError():
        ErrorBase(ERROR_TYPE)
{
}

const char* ParserError::what(void) const noexcept {
    return "Parser Error";
}

const std::string ParserError::ERROR_TYPE = "ParserError";


} // exception
} // utils
