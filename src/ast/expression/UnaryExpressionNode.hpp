#ifndef VNLC_UNARY_EXPRESSION_NODE_HPP
#define VNLC_UNARY_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/UnaryExpressionKind.hpp"
#include <memory>

namespace vnlc {
    class UnaryExpressionNode : public ExpressionNode {
    private:
        UnaryExpressionNode() = delete;

        UnaryExpressionKind kind;
        std::unique_ptr<ExpressionNode> operand;

    public:
        UnaryExpressionNode(UnaryExpressionKind kind, std::unique_ptr<ExpressionNode>&& operand, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const UnaryExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getOperand() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_UNARY_EXPRESSION_NODE_HPP