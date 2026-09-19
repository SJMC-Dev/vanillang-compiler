#include "ExpressionStatementNode.hpp"

namespace vnlc {
    ExpressionStatementNode::ExpressionStatementNode(std::unique_ptr<ExpressionNode>&& expression, const Token& firstToken, const Token& lastToken) noexcept
        : StatementNode(firstToken, lastToken),
          expression(std::move(expression)) {}

    const ExpressionNode& ExpressionStatementNode::getExpression() const noexcept {
        return *expression;
    }
} // namespace vnlc
