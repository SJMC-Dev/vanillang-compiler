#include "ExportDeclarationNode.hpp"

namespace vnlc {
    ExportDeclarationNode::ExportDeclarationNode(std::vector<ExportDeclarationItem>&& nameList, const Token& firstToken, const Token& lastToken) noexcept
        : DeclarationNode(firstToken, lastToken),
          nameList(std::move(nameList)) {}

    const std::vector<ExportDeclarationItem>& ExportDeclarationNode::getNameList() const noexcept {
        return nameList;
    }
} // namespace vnlc
