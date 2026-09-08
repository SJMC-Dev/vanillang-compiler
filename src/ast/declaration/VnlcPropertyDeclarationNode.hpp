#ifndef VNLC_PROPERTY_DECLARATION_NODE_HPP
#define VNLC_PROPERTY_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcPropertyDeclarationType.hpp"
#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"

class VnlcPropertyDeclarationNode : public VnlcDeclarationNode {
private:
    VnlcPropertyDeclarationNode() = delete;

    VnlcPropertyDeclarationType::AccessModifier accessModifier;
    VnlcPropertyDeclarationType::Binding binding;
    std::unique_ptr<VnlcIdentifierNode> name;
    std::unique_ptr<VnlcTypeNode> type;
    std::optional<std::unique_ptr<VnlcExpressionNode>> initializer;

public:
    VnlcPropertyDeclarationNode(
        VnlcPropertyDeclarationType::AccessModifier accessModifier,
        VnlcPropertyDeclarationType::Binding binding,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::unique_ptr<VnlcTypeNode>&& type,
        std::optional<std::unique_ptr<VnlcExpressionNode>>&& initializer,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcPropertyDeclarationNode(
        VnlcPropertyDeclarationType::AccessModifier accessModifier,
        VnlcPropertyDeclarationType::Binding binding,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::unique_ptr<VnlcTypeNode>&& type,
        std::optional<std::unique_ptr<VnlcExpressionNode>>&& initializer,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const VnlcPropertyDeclarationType::AccessModifier getAccessModifier() const noexcept;
    [[nodiscard]] const VnlcPropertyDeclarationType::Binding getBinding() const noexcept;
    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
    [[nodiscard]] const VnlcTypeNode& getType() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcExpressionNode>>& getInitializer() const noexcept;
};

#endif // VNLC_PROPERTY_DECLARATION_NODE_HPP