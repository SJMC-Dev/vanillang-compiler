#ifndef VNLC_IMPORT_DECLARATION_NODE_HPP
#define VNLC_IMPORT_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcImportDeclarationItem.hpp"

class VnlcImportDeclarationNode : public VnlcDeclarationNode {
private:
    VnlcImportDeclarationNode() = delete;

    std::unique_ptr<VnlcImportDeclarationItem> paths;

public:
    VnlcImportDeclarationNode(std::unique_ptr<VnlcImportDeclarationItem>&& paths, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const VnlcImportDeclarationItem& getNamePartsListWithAliases() const noexcept;
};

#endif // VNLC_IMPORT_DECLARATION_NODE_HPP