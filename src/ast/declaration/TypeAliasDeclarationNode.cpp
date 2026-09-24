#include "TypeAliasDeclarationNode.hpp"

namespace vnlc {
    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        std::unique_ptr<IdentifierNode>&& aliasName,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::unique_ptr<TypeReferenceNode>&& originalType,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          aliasName(std::move(aliasName)),
          genericParameterNames(std::move(genericParameterNames)),
          originalType(std::move(originalType)) {}

    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        std::unique_ptr<IdentifierNode>&& aliasName,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::unique_ptr<TypeReferenceNode>&& originalType,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          aliasName(std::move(aliasName)),
          genericParameterNames(std::move(genericParameterNames)),
          originalType(std::move(originalType)) {}

    const IdentifierNode& TypeAliasDeclarationNode::getAliasName() const noexcept {
        return *aliasName;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& TypeAliasDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const TypeReferenceNode& TypeAliasDeclarationNode::getOriginalType() const noexcept {
        return *originalType;
    }
} // namespace vnlc
