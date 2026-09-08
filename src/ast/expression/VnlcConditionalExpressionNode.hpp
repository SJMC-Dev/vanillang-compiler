#ifndef VNLC_CONDITIONAL_EXPRESSION_NODE_HPP
#define VNLC_CONDITIONAL_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

class VnlcConditionalExpressionNode : public VnlcExpressionNode {
private:
    VnlcConditionalExpressionNode() = delete;

    std::unique_ptr<VnlcExpressionNode> condition;
    std::unique_ptr<VnlcExpressionNode> thenExpression;
    std::unique_ptr<VnlcExpressionNode> elseExpression;

public:
    VnlcConditionalExpressionNode(
        std::unique_ptr<VnlcExpressionNode>&& condition,
        std::unique_ptr<VnlcExpressionNode>&& thenExpression,
        std::unique_ptr<VnlcExpressionNode>&& elseExpression,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcExpressionNode& getCondition() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getThenExpression() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getElseExpression() const noexcept;
};

#endif // VNLC_CONDITIONAL_EXPRESSION_NODE_HPP
