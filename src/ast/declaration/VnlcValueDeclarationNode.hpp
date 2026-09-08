#ifndef VNLC_VALUE_DECLARATION_NODE_HPP
#define VNLC_VALUE_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationType.hpp"
#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <memory>

class VnlcValueDeclarationNode : public VnlcDeclarationNode {
private:
    VnlcValueDeclarationNode() = delete;

    VnlcValueDeclarationType::Kind kind;
    VnlcValueDeclarationType::Context context;
    VnlcValueDeclarationType::AccessModifier accessModifier;
    std::unique_ptr<VnlcIdentifierNode> name;
    std::optional<std::unique_ptr<VnlcTypeNode>> type;
    std::optional<std::unique_ptr<VnlcExpressionNode>> initializer;

public:
    VnlcValueDeclarationNode(
        VnlcValueDeclarationType::Kind kind,
        VnlcValueDeclarationType::Context context,
        VnlcValueDeclarationType::AccessModifier accessModifier,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& type,
        std::optional<std::unique_ptr<VnlcExpressionNode>>&& initializer,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcValueDeclarationNode(
        VnlcValueDeclarationType::Kind kind,
        VnlcValueDeclarationType::Context context,
        VnlcValueDeclarationType::AccessModifier accessModifier,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& type,
        std::optional<std::unique_ptr<VnlcExpressionNode>>&& initializer,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const VnlcValueDeclarationType::Kind getKind() const noexcept;
    [[nodiscard]] const VnlcValueDeclarationType::Context getContext() const noexcept;
    [[nodiscard]] const VnlcValueDeclarationType::AccessModifier getAccessModifier() const noexcept;
    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcTypeNode>>& getType() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcExpressionNode>>& getInitializer() const noexcept;
};

#endif // VNLC_VALUE_DECLARATION_NODE_HPP