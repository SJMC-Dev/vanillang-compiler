#ifndef VNLC_EXPRESSION_NODE_HPP
#define VNLC_EXPRESSION_NODE_HPP

#include "ast/VnlcAstNode.hpp"

class VnlcExpressionNode : public VnlcAstNode {
private:
    VnlcExpressionNode() = delete;

protected:
    VnlcExpressionNode(const VnlcToken& firstToken, const VnlcToken& lastToken);
};

#endif // VNLC_EXPRESSION_NODE_HPP