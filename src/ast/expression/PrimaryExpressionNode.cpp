#include "PrimaryExpressionNode.hpp"

namespace vnlc {
    PrimaryExpressionNode::PrimaryExpressionNode(const Token& firstToken, const Token& lastToken) noexcept : ExpressionNode(firstToken, lastToken) {}
} // namespace vnlc
