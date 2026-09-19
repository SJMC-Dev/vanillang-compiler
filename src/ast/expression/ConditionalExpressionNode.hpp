#ifndef VNLC_CONDITIONAL_EXPRESSION_NODE_HPP
#define VNLC_CONDITIONAL_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class ConditionalExpressionNode : public ExpressionNode {
    private:
        ConditionalExpressionNode() = delete;

        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<ExpressionNode> thenExpression;
        std::unique_ptr<ExpressionNode> elseExpression;

    public:
        ConditionalExpressionNode(
            std::unique_ptr<ExpressionNode>&& condition,
            std::unique_ptr<ExpressionNode>&& thenExpression,
            std::unique_ptr<ExpressionNode>&& elseExpression,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getCondition() const noexcept;
        [[nodiscard]] const ExpressionNode& getThenExpression() const noexcept;
        [[nodiscard]] const ExpressionNode& getElseExpression() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CONDITIONAL_EXPRESSION_NODE_HPP
