#include "VnlcExportDeclarationNode.hpp"

VnlcExportDeclarationNode::VnlcExportDeclarationNode(std::vector<VnlcExportDeclarationItem>&& nameList, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept
    : VnlcDeclarationNode(firstToken, lastToken),
      nameList(std::move(nameList)) {}

const std::vector<VnlcExportDeclarationItem>& VnlcExportDeclarationNode::getNameList() const noexcept {
    return nameList;
}