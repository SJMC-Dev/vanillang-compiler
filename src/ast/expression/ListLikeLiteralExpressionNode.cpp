#include "ListLikeLiteralExpressionNode.hpp"

namespace vnlc {
    ListLikeLiteralExpressionNode::ListLikeLiteralExpressionNode(
        ListLikeLiteralExpressionType type,
        std::vector<std::unique_ptr<ExpressionNode>>&& elements,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          type(type),
          elements(std::move(elements)) {}

    const ListLikeLiteralExpressionType ListLikeLiteralExpressionNode::getType() const noexcept {
        return type;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& ListLikeLiteralExpressionNode::getElements() const noexcept {
        return elements;
    }
} // namespace vnlc
