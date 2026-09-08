#ifndef VNLC_SUBSCRIPT_EXPRESSION_NODE_HPP
#define VNLC_SUBSCRIPT_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

class VnlcSubscriptExpressionNode : public VnlcExpressionNode {
private:
    VnlcSubscriptExpressionNode() = delete;

    std::unique_ptr<VnlcExpressionNode> object;
    std::unique_ptr<VnlcExpressionNode> index;

public:
    VnlcSubscriptExpressionNode(std::unique_ptr<VnlcExpressionNode>&& object, std::unique_ptr<VnlcExpressionNode>&& index, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const VnlcExpressionNode& getObject() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getIndex() const noexcept;
};

#endif // VNLC_SUBSCRIPT_EXPRESSION_NODE_HPP