#ifndef VNLC_EXPORT_DECLARATION_ITEM_HPP
#define VNLC_EXPORT_DECLARATION_ITEM_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>
#include <optional>

struct VnlcExportDeclarationItem {
    std::unique_ptr<VnlcIdentifierNode> name;
    std::optional<std::unique_ptr<VnlcIdentifierNode>> alias; // nullopt if no alias
};

#endif // VNLC_EXPORT_DECLARATION_ITEM_HPP