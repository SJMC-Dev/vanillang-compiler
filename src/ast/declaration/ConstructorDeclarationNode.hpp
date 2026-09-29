#ifndef VNLC_CONSTRUCTOR_DECLARATION_NODE_HPP
#define VNLC_CONSTRUCTOR_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationKind.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class ConstructorDeclarationNode : public DeclarationNode {
    private:
        ConstructorDeclarationNode() = delete;

        FunctionDeclarationKind::AccessModifier accessModifier;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::unique_ptr<BlockStatementNode> body;

    public:
        ConstructorDeclarationNode(
            FunctionDeclarationKind::AccessModifier accessModifier,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::unique_ptr<BlockStatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        ConstructorDeclarationNode(
            FunctionDeclarationKind::AccessModifier accessModifier,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::unique_ptr<BlockStatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const FunctionDeclarationKind::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getParameters() const noexcept;
        [[nodiscard]] const BlockStatementNode& getBody() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CONSTRUCTOR_DECLARATION_NODE_HPP
