#ifndef VNLC_ILLEGAL_MODULE_OR_PACKAGE_NAME_ERROR_HPP
#define VNLC_ILLEGAL_MODULE_OR_PACKAGE_NAME_ERROR_HPP

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class IllegalModuleOrPackageNameError : public Error {
    public:
        IllegalModuleOrPackageNameError(std::string_view message) : Error(fmt::format("Illegal module or package name: {}", message)) {}
    };
} // namespace vnlc

#endif // VNLC_ILLEGAL_MODULE_OR_PACKAGE_NAME_ERROR_HPP