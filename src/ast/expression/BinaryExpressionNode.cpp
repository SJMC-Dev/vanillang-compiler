#include "BinaryExpressionNode.hpp"
#include <utility>

namespace vnlc {
    BinaryExpressionNode::BinaryExpressionNode(
        BinaryExpressionType type,
        std::unique_ptr<ExpressionNode>&& left,
        std::unique_ptr<ExpressionNode>&& right,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          type(type),
          left(std::move(left)),
          right(std::move(right)) {}

    const BinaryExpressionType BinaryExpressionNode::getType() const noexcept {
        return type;
    }

    const ExpressionNode& BinaryExpressionNode::getLeft() const noexcept {
        return *left;
    }

    const ExpressionNode& BinaryExpressionNode::getRight() const noexcept {
        return *right;
    }
} // namespace vnlc
