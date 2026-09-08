#ifndef VNLC_ILLEGAL_MODULE_OR_PACKAGE_NAME_ERROR_HPP
#define VNLC_ILLEGAL_MODULE_OR_PACKAGE_NAME_ERROR_HPP

#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcIllegalModuleOrPackageNameError : public VnlcError {
public:
    VnlcIllegalModuleOrPackageNameError(std::string_view message) : VnlcError(fmt::format("Illegal module or package name: {}", message)) {}
};

#endif // VNLC_ILLEGAL_MODULE_OR_PACKAGE_NAME_ERROR_HPP