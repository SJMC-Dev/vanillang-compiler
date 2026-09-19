#ifndef VNLC_AST_CONSTRUCTION_ERROR_HPP
#define VNLC_AST_CONSTRUCTION_ERROR_HPP

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class AstConstructionError : public Error {
    public:
        AstConstructionError(std::string_view message) : Error(fmt::format("Invalid AST construction: {}", message)) {}
    };
} // namespace vnlc

#endif // VNLC_AST_CONSTRUCTION_ERROR_HPP