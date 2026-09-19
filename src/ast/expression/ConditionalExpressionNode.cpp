#include "ConditionalExpressionNode.hpp"

namespace vnlc {
    ConditionalExpressionNode::ConditionalExpressionNode(
        std::unique_ptr<ExpressionNode>&& condition,
        std::unique_ptr<ExpressionNode>&& thenExpression,
        std::unique_ptr<ExpressionNode>&& elseExpression,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          condition(std::move(condition)),
          thenExpression(std::move(thenExpression)),
          elseExpression(std::move(elseExpression)) {}

    const ExpressionNode& ConditionalExpressionNode::getCondition() const noexcept {
        return *condition;
    }

    const ExpressionNode& ConditionalExpressionNode::getThenExpression() const noexcept {
        return *thenExpression;
    }

    const ExpressionNode& ConditionalExpressionNode::getElseExpression() const noexcept {
        return *elseExpression;
    }
} // namespace vnlc
