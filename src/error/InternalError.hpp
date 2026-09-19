#ifndef VNLC_INTERNAL_ERROR_HPP
#define VNLC_INTERNAL_ERROR_HPP

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class InternalError : public Error {
    public:
        InternalError(std::string_view message) : Error(fmt::format("Internal error: {}", message)) {}
    };
} // namespace vnlc

#endif // VNLC_INTERNAL_ERROR_HPP