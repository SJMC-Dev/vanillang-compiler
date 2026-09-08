#ifndef VNLC_RANGE_EXPRESSION_NODE_HPP
#define VNLC_RANGE_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>
#include <optional>

class VnlcRangeExpressionNode : public VnlcExpressionNode {
private:
    VnlcRangeExpressionNode() = delete;

    std::optional<std::unique_ptr<VnlcExpressionNode>> start;
    std::optional<std::unique_ptr<VnlcExpressionNode>> end;

public:
    VnlcRangeExpressionNode(
        std::optional<std::unique_ptr<VnlcExpressionNode>>&& start,
        std::optional<std::unique_ptr<VnlcExpressionNode>>&& end,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const std::optional<std::unique_ptr<VnlcExpressionNode>>& getStart() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcExpressionNode>>& getEnd() const noexcept;
};

#endif // VNLC_RANGE_EXPRESSION_NODE_HPP