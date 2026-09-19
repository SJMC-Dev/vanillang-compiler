#include "EnumMemberDeclarationNode.hpp"

namespace vnlc {
    EnumMemberDeclarationNode::EnumMemberDeclarationNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(firstToken, lastToken),
          name(std::move(name)),
          associatedValues(std::move(associatedValues)) {}

    EnumMemberDeclarationNode::EnumMemberDeclarationNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          name(std::move(name)),
          associatedValues(std::move(associatedValues)) {}

    const IdentifierNode& EnumMemberDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& EnumMemberDeclarationNode::getAssociatedValues() const noexcept {
        return associatedValues;
    }
} // namespace vnlc
