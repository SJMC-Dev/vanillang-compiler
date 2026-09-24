#include "NoneExpressionNode.hpp"

namespace vnlc {
    NoneExpressionNode::NoneExpressionNode(const Token& firstToken, const Token& lastToken) noexcept : PrimaryExpressionNode(firstToken, lastToken) {}
} // namespace vnlc
