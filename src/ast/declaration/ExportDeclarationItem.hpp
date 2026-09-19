#ifndef VNLC_EXPORT_DECLARATION_ITEM_HPP
#define VNLC_EXPORT_DECLARATION_ITEM_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>

namespace vnlc {
    struct ExportDeclarationItem {
        std::unique_ptr<IdentifierNode> name;
    };
} // namespace vnlc

#endif // VNLC_EXPORT_DECLARATION_ITEM_HPP