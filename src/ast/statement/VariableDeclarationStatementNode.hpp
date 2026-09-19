#ifndef VNLC_VARIABLE_DECLARATION_STATEMENT_NODE_HPP
#define VNLC_VARIABLE_DECLARATION_STATEMENT_NODE_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include <memory>

namespace vnlc {
    class VariableDeclarationStatementNode : public StatementNode {
    private:
        VariableDeclarationStatementNode() = delete;

        std::unique_ptr<ValueDeclarationNode> variableDeclaration;

    public:
        VariableDeclarationStatementNode(std::unique_ptr<ValueDeclarationNode>&& variableDeclaration, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ValueDeclarationNode& getVariableDeclaration() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_STATEMENT_NODE_HPP