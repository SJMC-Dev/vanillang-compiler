#include "UnaryExpressionNode.hpp"
#include <utility>

namespace vnlc {
    UnaryExpressionNode::UnaryExpressionNode(UnaryExpressionKind kind, std::unique_ptr<ExpressionNode>&& operand, const Token& firstToken, const Token& lastToken) noexcept
        : ExpressionNode(firstToken, lastToken),
          kind(kind),
          operand(std::move(operand)) {}

    const UnaryExpressionKind UnaryExpressionNode::getKind() const noexcept {
        return kind;
    }

    const ExpressionNode& UnaryExpressionNode::getOperand() const noexcept {
        return *operand;
    }
} // namespace vnlc
