#ifndef VNLC_EXPRESSION_NODE_HPP
#define VNLC_EXPRESSION_NODE_HPP

#include "ast/AstNode.hpp"

namespace vnlc {
    class ExpressionNode : public AstNode {
    private:
        ExpressionNode() = delete;

    protected:
        ExpressionNode(const Token& firstToken, const Token& lastToken);
    };
} // namespace vnlc

#endif // VNLC_EXPRESSION_NODE_HPP