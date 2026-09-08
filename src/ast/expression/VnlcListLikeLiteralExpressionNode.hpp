#ifndef VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcListLikeLiteralExpressionType.hpp"
#include "ast/expression/VnlcLiteralExpressionNode.hpp"
#include <memory>
#include <vector>

class VnlcListLikeLiteralExpressionNode : public VnlcLiteralExpressionNode {
private:
    VnlcListLikeLiteralExpressionNode() = delete;

    VnlcListLikeLiteralExpressionType type;
    std::vector<std::unique_ptr<VnlcExpressionNode>> elements;

public:
    VnlcListLikeLiteralExpressionNode(
        VnlcListLikeLiteralExpressionType type,
        std::vector<std::unique_ptr<VnlcExpressionNode>>&& elements,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcListLikeLiteralExpressionType getType() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcExpressionNode>>& getElements() const noexcept;
};

#endif // VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP
