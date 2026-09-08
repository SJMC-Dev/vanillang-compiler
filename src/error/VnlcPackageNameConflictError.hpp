#ifndef VNLC_PACKAGE_NAME_CONFLICT_ERROR_HPP
#define VNLC_PACKAGE_NAME_CONFLICT_ERROR_HPP

#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcPackageNameConflictError : public VnlcError {
public:
    VnlcPackageNameConflictError(std::string_view packageName) : VnlcError(fmt::format("Found two or more packages with the name: {}", packageName)) {}
};

#endif // VNLC_PACKAGE_NAME_CONFLICT_ERROR_HPP