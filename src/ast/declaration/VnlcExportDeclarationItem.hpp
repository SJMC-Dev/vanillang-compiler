#ifndef VNLC_EXPORT_DECLARATION_ITEM_HPP
#define VNLC_EXPORT_DECLARATION_ITEM_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>

struct VnlcExportDeclarationItem {
    std::unique_ptr<VnlcIdentifierNode> name;
};

#endif // VNLC_EXPORT_DECLARATION_ITEM_HPP