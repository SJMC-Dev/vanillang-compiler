#ifndef VNLC_FUNCTION_DECLARATION_NODE_HPP
#define VNLC_FUNCTION_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcFunctionDeclarationType.hpp"
#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "ast/statement/VnlcBlockStatementNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <memory>
#include <optional>

class VnlcFunctionDeclarationNode : public VnlcDeclarationNode {
private:
    VnlcFunctionDeclarationNode() = delete;

    VnlcFunctionDeclarationType::Kind kind;
    VnlcFunctionDeclarationType::Context context;
    VnlcFunctionDeclarationType::AccessModifier accessModifier;
    VnlcFunctionDeclarationType::Binding binding;

    std::unique_ptr<VnlcIdentifierNode> name;
    std::vector<std::unique_ptr<VnlcValueDeclarationNode>> parameters;
    std::optional<std::unique_ptr<VnlcTypeNode>> returnType;
    std::optional<std::unique_ptr<VnlcBlockStatementNode>> body;

public:
    VnlcFunctionDeclarationNode(
        VnlcFunctionDeclarationType::Kind kind,
        VnlcFunctionDeclarationType::Context context,
        VnlcFunctionDeclarationType::AccessModifier accessModifier,
        VnlcFunctionDeclarationType::Binding binding,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& returnType,
        std::optional<std::unique_ptr<VnlcBlockStatementNode>>&& body,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcFunctionDeclarationNode(
        VnlcFunctionDeclarationType::Kind kind,
        VnlcFunctionDeclarationType::Context context,
        VnlcFunctionDeclarationType::AccessModifier accessModifier,
        VnlcFunctionDeclarationType::Binding binding,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& returnType,
        std::optional<std::unique_ptr<VnlcBlockStatementNode>>&& body,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const VnlcFunctionDeclarationType::Kind getKind() const noexcept;
    [[nodiscard]] const VnlcFunctionDeclarationType::Context getContext() const noexcept;
    [[nodiscard]] const VnlcFunctionDeclarationType::AccessModifier getAccessModifier() const noexcept;
    [[nodiscard]] const VnlcFunctionDeclarationType::Binding getBinding() const noexcept;
    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcValueDeclarationNode>>& getParameters() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcTypeNode>>& getReturnType() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcBlockStatementNode>>& getBody() const noexcept;
};

#endif // VNLC_FUNCTION_DECLARATION_NODE_HPP