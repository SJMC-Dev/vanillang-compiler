#ifndef VNLC_UNARY_EXPRESSION_NODE_HPP
#define VNLC_UNARY_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/expression/VnlcUnaryExpressionType.hpp"
#include <memory>

class VnlcUnaryExpressionNode : public VnlcExpressionNode {
private:
    VnlcUnaryExpressionNode() = delete;

    VnlcUnaryExpressionType type;
    std::unique_ptr<VnlcExpressionNode> operand;

public:
    VnlcUnaryExpressionNode(VnlcUnaryExpressionType type, std::unique_ptr<VnlcExpressionNode>&& operand, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const VnlcUnaryExpressionType getType() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getOperand() const noexcept;
};

#endif // VNLC_UNARY_EXPRESSION_NODE_HPP