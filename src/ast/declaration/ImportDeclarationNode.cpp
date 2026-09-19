#include "ImportDeclarationNode.hpp"

namespace vnlc {
    ImportDeclarationNode::ImportDeclarationNode(std::unique_ptr<ImportDeclarationItem>&& paths, const Token& firstToken, const Token& lastToken) noexcept
        : DeclarationNode(firstToken, lastToken),
          paths(std::move(paths)) {}

    const ImportDeclarationItem& ImportDeclarationNode::getNamePartsListWithAliases() const noexcept {
        return *paths;
    }
} // namespace vnlc
