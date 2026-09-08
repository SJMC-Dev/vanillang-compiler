#ifndef VNLC_SYNTAX_ERROR_HPP
#define VNLC_SYNTAX_ERROR_HPP

#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcSyntaxError : public VnlcError {
public:
    VnlcSyntaxError(std::string_view message, int line, int column) : VnlcError(fmt::format("Syntax error at line {}, column {}: {}", line, column, message)) {}
};

#endif // VNLC_SYNTAX_ERROR_HPP