#include "SubscriptExpressionNode.hpp"

namespace vnlc {
    SubscriptExpressionNode::SubscriptExpressionNode(
        std::unique_ptr<ExpressionNode>&& object,
        std::unique_ptr<ExpressionNode>&& index,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          object(std::move(object)),
          index(std::move(index)) {}

    const ExpressionNode& SubscriptExpressionNode::getObject() const noexcept {
        return *object;
    }

    const ExpressionNode& SubscriptExpressionNode::getIndex() const noexcept {
        return *index;
    }
} // namespace vnlc
