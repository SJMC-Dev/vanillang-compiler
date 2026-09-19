#ifndef VNLC_ILLEGAL_INPUT_ERROR_HPP
#define VNLC_ILLEGAL_INPUT_ERROR_HPP

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class IllegalInputError : public Error {
    public:
        IllegalInputError(std::string_view message) : Error(fmt::format("Illegal input: {}", message)) {}
    };
} // namespace vnlc

#endif // VNLC_ILLEGAL_INPUT_ERROR_HPP