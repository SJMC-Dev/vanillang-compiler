#ifndef VNLC_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class LiteralExpressionNode : public PrimaryExpressionNode {
    private:
        LiteralExpressionNode() = delete;

    protected:
        LiteralExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_LITERAL_EXPRESSION_NODE_HPP
