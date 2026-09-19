#ifndef VNLC_PACKAGE_READER_ERROR_HPP
#define VNLC_PACKAGE_READER_ERROR_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class PackageReaderError : public Error {
    private:
        const IdentifierNode* identifierNode;

    public:
        PackageReaderError(std::string_view message, const IdentifierNode* identifierNode = nullptr)
            : Error(fmt::format("Error reading package: {}", message)),
              identifierNode(identifierNode) {}

        [[nodiscard]] const IdentifierNode* locate() const {
            return identifierNode;
        }
    };
} // namespace vnlc

#endif // VNLC_PACKAGE_READER_ERROR_HPP