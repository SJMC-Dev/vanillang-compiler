#ifndef VNLC_EXPRESSION_STATEMENT_NODE_HPP
#define VNLC_EXPRESSION_STATEMENT_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include <memory>

namespace vnlc {
    class ExpressionStatementNode : public StatementNode {
    private:
        ExpressionStatementNode() = delete;

        std::unique_ptr<ExpressionNode> expression;

    public:
        ExpressionStatementNode(std::unique_ptr<ExpressionNode>&& expression, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode& getExpression() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_EXPRESSION_STATEMENT_NODE_HPP
