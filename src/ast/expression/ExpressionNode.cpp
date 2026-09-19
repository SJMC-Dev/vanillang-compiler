#include "ExpressionNode.hpp"

namespace vnlc {
    ExpressionNode::ExpressionNode(const Token& firstToken, const Token& lastToken) : AstNode(firstToken, lastToken) {}
} // namespace vnlc
