#ifndef VNLC_MODULE_INTERFACE_FILE_READER_ERROR_HPP
#define VNLC_MODULE_INTERFACE_FILE_READER_ERROR_HPP

#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcModuleInterfaceFileReaderError : public VnlcError {
public:
    VnlcModuleInterfaceFileReaderError(std::string_view message) : VnlcError(fmt::format("Error reading module interface file: {}", message)) {}
};

#endif // VNLC_MODULE_INTERFACE_FILE_READER_ERROR_HPP