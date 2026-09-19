#include "LiteralExpressionNode.hpp"

namespace vnlc {
    LiteralExpressionNode::LiteralExpressionNode(const Token& firstToken, const Token& lastToken) noexcept : PrimaryExpressionNode(firstToken, lastToken) {}
} // namespace vnlc
