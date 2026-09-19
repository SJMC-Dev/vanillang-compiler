#include "PropertyDeclarationNode.hpp"

namespace vnlc {
    PropertyDeclarationNode::PropertyDeclarationNode(
        PropertyDeclarationType::AccessModifier accessModifier,
        PropertyDeclarationType::Binding binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeNode>&& type,
        std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
          type(std::move(type)),
          initializer(std::move(initializer)) {}

    PropertyDeclarationNode::PropertyDeclarationNode(
        PropertyDeclarationType::AccessModifier accessModifier,
        PropertyDeclarationType::Binding binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeNode>&& type,
        std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
          type(std::move(type)),
          initializer(std::move(initializer)) {}

    const PropertyDeclarationType::AccessModifier PropertyDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const PropertyDeclarationType::Binding PropertyDeclarationNode::getBinding() const noexcept {
        return binding;
    }

    const IdentifierNode& PropertyDeclarationNode::getName() const noexcept {
        return *name;
    }

    const TypeNode& PropertyDeclarationNode::getType() const noexcept {
        return *type;
    }

    const std::optional<std::unique_ptr<ExpressionNode>>& PropertyDeclarationNode::getInitializer() const noexcept {
        return initializer;
    }
} // namespace vnlc
