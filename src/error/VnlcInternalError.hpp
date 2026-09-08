#ifndef VNLC_INTERNAL_ERROR_HPP
#define VNLC_INTERNAL_ERROR_HPP

#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcInternalError : public VnlcError {
public:
    VnlcInternalError(std::string_view message) : VnlcError(fmt::format("Internal error: {}", message)) {}
};

#endif // VNLC_INTERNAL_ERROR_HPP