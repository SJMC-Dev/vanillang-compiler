#include "ListLikeLiteralExpressionNode.hpp"

namespace vnlc {
    ListLikeLiteralExpressionNode::ListLikeLiteralExpressionNode(
        ListLikeLiteralExpressionKind kind,
        std::vector<std::unique_ptr<ExpressionNode>>&& elements,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          elements(std::move(elements)) {}

    const ListLikeLiteralExpressionKind ListLikeLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& ListLikeLiteralExpressionNode::getElements() const noexcept {
        return elements;
    }
} // namespace vnlc
