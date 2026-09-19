#ifndef VNLC_MODULE_INTERFACE_FILE_READER_ERROR_HPP
#define VNLC_MODULE_INTERFACE_FILE_READER_ERROR_HPP

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class ModuleInterfaceFileReaderError : public Error {
    public:
        ModuleInterfaceFileReaderError(std::string_view message) : Error(fmt::format("Error reading module interface file: {}", message)) {}
    };
} // namespace vnlc

#endif // VNLC_MODULE_INTERFACE_FILE_READER_ERROR_HPP