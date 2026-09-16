#ifndef VNLC_EXPORT_DECLARATION_NODE_HPP
#define VNLC_EXPORT_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcExportDeclarationItem.hpp"
#include <vector>

class VnlcExportDeclarationNode : public VnlcDeclarationNode {
private:
    VnlcExportDeclarationNode() = delete;

    std::vector<VnlcExportDeclarationItem> nameList;

public:
    VnlcExportDeclarationNode(std::vector<VnlcExportDeclarationItem>&& nameList, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const std::vector<VnlcExportDeclarationItem>& getNameList() const noexcept;
};

#endif // VNLC_EXPORT_DECLARATION_NODE_HPP