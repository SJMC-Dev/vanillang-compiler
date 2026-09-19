#ifndef VNLC_FOR_STATEMENT_NODE_HPP
#define VNLC_FOR_STATEMENT_NODE_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/LoopStatementNode.hpp"
#include <memory>

namespace vnlc {
    class ForStatementNode : public LoopStatementNode {
    private:
        ForStatementNode() = delete;

        std::unique_ptr<ValueDeclarationNode> loopVariable;
        std::unique_ptr<ExpressionNode> iterableExpression;
        std::unique_ptr<StatementNode> body;

    public:
        ForStatementNode(
            std::unique_ptr<ValueDeclarationNode>&& loopVariable,
            std::unique_ptr<ExpressionNode>&& iterableExpression,
            std::unique_ptr<StatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        ForStatementNode(
            std::unique_ptr<IdentifierNode>&& label,
            std::unique_ptr<ValueDeclarationNode>&& loopVariable,
            std::unique_ptr<ExpressionNode>&& iterableExpression,
            std::unique_ptr<StatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ValueDeclarationNode& getLoopVariable() const noexcept;
        [[nodiscard]] const ExpressionNode& getIterableExpression() const noexcept;
        [[nodiscard]] const StatementNode& getBody() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_FOR_STATEMENT_NODE_HPP