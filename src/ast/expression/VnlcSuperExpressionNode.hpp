#ifndef VNLC_SUPER_EXPRESSION_NODE_HPP
#define VNLC_SUPER_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcPrimaryExpressionNode.hpp"

class VnlcSuperExpressionNode : public VnlcPrimaryExpressionNode {
private:
    VnlcSuperExpressionNode() = delete;

public:
    VnlcSuperExpressionNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
};

#endif // VNLC_SUPER_EXPRESSION_NODE_HPP