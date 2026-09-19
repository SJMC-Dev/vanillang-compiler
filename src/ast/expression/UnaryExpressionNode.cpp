#include "UnaryExpressionNode.hpp"
#include <utility>

namespace vnlc {
    UnaryExpressionNode::UnaryExpressionNode(UnaryExpressionType type, std::unique_ptr<ExpressionNode>&& operand, const Token& firstToken, const Token& lastToken) noexcept
        : ExpressionNode(firstToken, lastToken),
          type(type),
          operand(std::move(operand)) {}

    const UnaryExpressionType UnaryExpressionNode::getType() const noexcept {
        return type;
    }

    const ExpressionNode& UnaryExpressionNode::getOperand() const noexcept {
        return *operand;
    }
} // namespace vnlc
