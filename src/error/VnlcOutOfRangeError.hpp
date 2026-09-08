#ifndef VNLC_OUT_OF_RANGE_ERROR_HPP
#define VNLC_OUT_OF_RANGE_ERROR_HPP

#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcOutOfRangeError : public VnlcError {
public:
    VnlcOutOfRangeError(std::string_view message) : VnlcError(fmt::format("Out of range: {}", message)) {}
};

#endif // VNLC_OUT_OF_RANGE_ERROR_HPP