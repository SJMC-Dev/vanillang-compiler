#ifndef VNLC_PRIMARY_EXPRESSION_NODE_HPP
#define VNLC_PRIMARY_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"

class VnlcPrimaryExpressionNode : public VnlcExpressionNode {
private:
    VnlcPrimaryExpressionNode() = delete;

protected:
    VnlcPrimaryExpressionNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
};

#endif // VNLC_PRIMARY_EXPRESSION_NODE_HPP
