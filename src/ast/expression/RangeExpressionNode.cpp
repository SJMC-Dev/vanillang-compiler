#include "RangeExpressionNode.hpp"

namespace vnlc {
    RangeExpressionNode::RangeExpressionNode(
        std::optional<std::unique_ptr<ExpressionNode>>&& start,
        std::optional<std::unique_ptr<ExpressionNode>>&& end,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          start(std::move(start)),
          end(std::move(end)) {}

    const std::optional<std::unique_ptr<ExpressionNode>>& RangeExpressionNode::getStart() const noexcept {
        return start;
    }

    const std::optional<std::unique_ptr<ExpressionNode>>& RangeExpressionNode::getEnd() const noexcept {
        return end;
    }
} // namespace vnlc
