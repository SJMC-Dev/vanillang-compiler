#ifndef VNLC_AST_CONSTRUCTION_ERROR_HPP
#define VNLC_AST_CONSTRUCTION_ERROR_HPP

#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcAstConstructionError : public VnlcError {
public:
    VnlcAstConstructionError(std::string_view message) : VnlcError(fmt::format("Invalid AST construction: {}", message)) {}
};

#endif // VNLC_AST_CONSTRUCTION_ERROR_HPP