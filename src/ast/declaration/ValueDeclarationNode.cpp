#include "ValueDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationType.hpp"

namespace vnlc {
    ValueDeclarationNode::ValueDeclarationNode(
        ValueDeclarationType::Kind kind,
        ValueDeclarationType::Context context,
        ValueDeclarationType::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::optional<std::unique_ptr<TypeNode>>&& type,
        std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(firstToken, lastToken),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          name(std::move(name)),
          type(std::move(type)),
          initializer(std::move(initializer)) {}

    ValueDeclarationNode::ValueDeclarationNode(
        ValueDeclarationType::Kind kind,
        ValueDeclarationType::Context context,
        ValueDeclarationType::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::optional<std::unique_ptr<TypeNode>>&& type,
        std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          name(std::move(name)),
          type(std::move(type)),
          initializer(std::move(initializer)) {}

    const ValueDeclarationType::Kind ValueDeclarationNode::getKind() const noexcept {
        return kind;
    }

    const ValueDeclarationType::Context ValueDeclarationNode::getContext() const noexcept {
        return context;
    }

    const ValueDeclarationType::AccessModifier ValueDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& ValueDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::optional<std::unique_ptr<TypeNode>>& ValueDeclarationNode::getType() const noexcept {
        return type;
    }

    const std::optional<std::unique_ptr<ExpressionNode>>& ValueDeclarationNode::getInitializer() const noexcept {
        return initializer;
    }
} // namespace vnlc
