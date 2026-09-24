#include "BinaryExpressionNode.hpp"
#include <utility>

namespace vnlc {
    BinaryExpressionNode::BinaryExpressionNode(
        BinaryExpressionKind kind,
        std::unique_ptr<ExpressionNode>&& left,
        std::unique_ptr<ExpressionNode>&& right,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          kind(kind),
          left(std::move(left)),
          right(std::move(right)) {}

    const BinaryExpressionKind BinaryExpressionNode::getKind() const noexcept {
        return kind;
    }

    const ExpressionNode& BinaryExpressionNode::getLeft() const noexcept {
        return *left;
    }

    const ExpressionNode& BinaryExpressionNode::getRight() const noexcept {
        return *right;
    }
} // namespace vnlc
