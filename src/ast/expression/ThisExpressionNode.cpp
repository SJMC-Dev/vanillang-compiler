#include "ThisExpressionNode.hpp"

namespace vnlc {
    ThisExpressionNode::ThisExpressionNode(const Token& firstToken, const Token& lastToken) noexcept : PrimaryExpressionNode(firstToken, lastToken) {}
} // namespace vnlc
