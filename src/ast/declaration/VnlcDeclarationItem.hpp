#ifndef VNLC_DECLARATION_ITEM_HPP
#define VNLC_DECLARATION_ITEM_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>
#include <optional>
#include <string>

namespace VnlcDeclarationItem {
    struct MetadataTerm {
        std::unique_ptr<VnlcIdentifierNode> key;
        std::optional<std::string> value; // if value is not provided, it is considered as "true"
    };
} // namespace VnlcDeclarationItem

#endif // VNLC_DECLARATION_ITEM_HPP