#ifndef VNLC_BINARY_EXPRESSION_NODE_HPP
#define VNLC_BINARY_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcBinaryExpressionType.hpp"
#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

class VnlcBinaryExpressionNode : public VnlcExpressionNode {
private:
    VnlcBinaryExpressionNode() = delete;

    VnlcBinaryExpressionType type;
    std::unique_ptr<VnlcExpressionNode> left;
    std::unique_ptr<VnlcExpressionNode> right;

public:
    VnlcBinaryExpressionNode(
        VnlcBinaryExpressionType type,
        std::unique_ptr<VnlcExpressionNode>&& left,
        std::unique_ptr<VnlcExpressionNode>&& right,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcBinaryExpressionType getType() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getLeft() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getRight() const noexcept;
};

#endif // VNLC_BINARY_EXPRESSION_NODE_HPP
