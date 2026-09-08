#ifndef VNLC_PACKAGE_READER_ERROR_HPP
#define VNLC_PACKAGE_READER_ERROR_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include "error/VnlcError.hpp"
#include <fmt/core.h>

class VnlcPackageReaderError : public VnlcError {
private:
    const VnlcIdentifierNode* identifierNode;

public:
    VnlcPackageReaderError(std::string_view message, const VnlcIdentifierNode* identifierNode = nullptr)
        : VnlcError(fmt::format("Error reading package: {}", message)),
          identifierNode(identifierNode) {}

    [[nodiscard]] const VnlcIdentifierNode* locate() const {
        return identifierNode;
    }
};

#endif // VNLC_PACKAGE_READER_ERROR_HPP