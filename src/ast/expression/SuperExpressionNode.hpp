#ifndef VNLC_SUPER_EXPRESSION_NODE_HPP
#define VNLC_SUPER_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class SuperExpressionNode : public PrimaryExpressionNode {
    private:
        SuperExpressionNode() = delete;

    public:
        SuperExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_SUPER_EXPRESSION_NODE_HPP