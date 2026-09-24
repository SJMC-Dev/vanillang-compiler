#ifndef VNLC_FUNCTION_DECLARATION_NODE_HPP
#define VNLC_FUNCTION_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationKind.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include "ast/type/TypeNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    class FunctionDeclarationNode : public DeclarationNode {
    private:
        FunctionDeclarationNode() = delete;

        FunctionDeclarationKind::Kind kind;
        FunctionDeclarationKind::Context context;
        FunctionDeclarationKind::AccessModifier accessModifier;
        FunctionDeclarationKind::Binding binding;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::optional<std::unique_ptr<TypeNode>> returnType;
        std::optional<std::unique_ptr<BlockStatementNode>> body;

    public:
        FunctionDeclarationNode(
            FunctionDeclarationKind::Kind kind,
            FunctionDeclarationKind::Context context,
            FunctionDeclarationKind::AccessModifier accessModifier,
            FunctionDeclarationKind::Binding binding,
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::optional<std::unique_ptr<TypeNode>>&& returnType,
            std::optional<std::unique_ptr<BlockStatementNode>>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        FunctionDeclarationNode(
            FunctionDeclarationKind::Kind kind,
            FunctionDeclarationKind::Context context,
            FunctionDeclarationKind::AccessModifier accessModifier,
            FunctionDeclarationKind::Binding binding,
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::optional<std::unique_ptr<TypeNode>>&& returnType,
            std::optional<std::unique_ptr<BlockStatementNode>>&& body,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const FunctionDeclarationKind::Kind getKind() const noexcept;
        [[nodiscard]] const FunctionDeclarationKind::Context getContext() const noexcept;
        [[nodiscard]] const FunctionDeclarationKind::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const FunctionDeclarationKind::Binding getBinding() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getParameters() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<TypeNode>>& getReturnType() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<BlockStatementNode>>& getBody() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_FUNCTION_DECLARATION_NODE_HPP