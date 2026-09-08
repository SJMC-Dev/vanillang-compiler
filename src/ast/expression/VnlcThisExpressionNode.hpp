#ifndef VNLC_THIS_EXPRESSION_NODE_HPP
#define VNLC_THIS_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcPrimaryExpressionNode.hpp"

class VnlcThisExpressionNode : public VnlcPrimaryExpressionNode {
private:
    VnlcThisExpressionNode() = delete;

public:
    VnlcThisExpressionNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
};

#endif // VNLC_THIS_EXPRESSION_NODE_HPP