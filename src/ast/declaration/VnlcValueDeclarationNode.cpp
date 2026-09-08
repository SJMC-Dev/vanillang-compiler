#include "VnlcValueDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationType.hpp"

VnlcValueDeclarationNode::VnlcValueDeclarationNode(
    VnlcValueDeclarationType::Kind kind,
    VnlcValueDeclarationType::Context context,
    VnlcValueDeclarationType::AccessModifier accessModifier,
    std::unique_ptr<VnlcIdentifierNode>&& name,
    std::optional<std::unique_ptr<VnlcTypeNode>>&& type,
    std::optional<std::unique_ptr<VnlcExpressionNode>>&& initializer,
    const VnlcToken& firstToken,
    const VnlcToken& lastToken
) noexcept
    : VnlcDeclarationNode(firstToken, lastToken),
      kind(kind),
      context(context),
      accessModifier(accessModifier),
      name(std::move(name)),
      type(std::move(type)),
      initializer(std::move(initializer)) {}

VnlcValueDeclarationNode::VnlcValueDeclarationNode(
    VnlcValueDeclarationType::Kind kind,
    VnlcValueDeclarationType::Context context,
    VnlcValueDeclarationType::AccessModifier accessModifier,
    std::unique_ptr<VnlcIdentifierNode>&& name,
    std::optional<std::unique_ptr<VnlcTypeNode>>&& type,
    std::optional<std::unique_ptr<VnlcExpressionNode>>&& initializer,
    const VnlcToken& firstToken,
    const VnlcToken& lastToken,
    std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
) noexcept
    : VnlcDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
      kind(kind),
      context(context),
      accessModifier(accessModifier),
      name(std::move(name)),
      type(std::move(type)),
      initializer(std::move(initializer)) {}

const VnlcValueDeclarationType::Kind VnlcValueDeclarationNode::getKind() const noexcept {
    return kind;
}

const VnlcValueDeclarationType::Context VnlcValueDeclarationNode::getContext() const noexcept {
    return context;
}

const VnlcValueDeclarationType::AccessModifier VnlcValueDeclarationNode::getAccessModifier() const noexcept {
    return accessModifier;
}

const VnlcIdentifierNode& VnlcValueDeclarationNode::getName() const noexcept {
    return *name;
}

const std::optional<std::unique_ptr<VnlcTypeNode>>& VnlcValueDeclarationNode::getType() const noexcept {
    return type;
}

const std::optional<std::unique_ptr<VnlcExpressionNode>>& VnlcValueDeclarationNode::getInitializer() const noexcept {
    return initializer;
}