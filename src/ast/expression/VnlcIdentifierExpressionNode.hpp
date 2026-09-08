#ifndef VNLC_IDENTIFIER_EXPRESSION_NODE_HPP
#define VNLC_IDENTIFIER_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcPrimaryExpressionNode.hpp"
#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>

class VnlcIdentifierExpressionNode : public VnlcPrimaryExpressionNode {
private:
    VnlcIdentifierExpressionNode() = delete;

    std::unique_ptr<VnlcIdentifierNode> name;

public:
    VnlcIdentifierExpressionNode(std::unique_ptr<VnlcIdentifierNode>&& name, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
};

#endif // VNLC_IDENTIFIER_EXPRESSION_NODE_HPP
