#ifndef VNLC_RANGE_EXPRESSION_NODE_HPP
#define VNLC_RANGE_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    class RangeExpressionNode : public ExpressionNode {
    private:
        RangeExpressionNode() = delete;

        std::optional<std::unique_ptr<ExpressionNode>> start;
        std::optional<std::unique_ptr<ExpressionNode>> end;

    public:
        RangeExpressionNode(
            std::optional<std::unique_ptr<ExpressionNode>>&& start,
            std::optional<std::unique_ptr<ExpressionNode>>&& end,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const std::optional<std::unique_ptr<ExpressionNode>>& getStart() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<ExpressionNode>>& getEnd() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_RANGE_EXPRESSION_NODE_HPP