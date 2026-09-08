#ifndef VNLC_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcPrimaryExpressionNode.hpp"

class VnlcLiteralExpressionNode : public VnlcPrimaryExpressionNode {
private:
    VnlcLiteralExpressionNode() = delete;

protected:
    VnlcLiteralExpressionNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
};

#endif // VNLC_LITERAL_EXPRESSION_NODE_HPP
