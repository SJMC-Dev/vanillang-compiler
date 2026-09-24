#include "PrimitiveTypeExpressionNode.hpp"

namespace vnlc {
    PrimitiveTypeExpressionNode::PrimitiveTypeExpressionNode(PrimitiveTypeExpressionKind kind, const Token& firstToken, const Token& lastToken) noexcept
        : PrimaryExpressionNode(firstToken, lastToken),
          kind(kind) {}

    const PrimitiveTypeExpressionKind PrimitiveTypeExpressionNode::getKind() const noexcept {
        return kind;
    }
} // namespace vnlc
