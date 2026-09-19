#ifndef VNLC_UNARY_EXPRESSION_NODE_HPP
#define VNLC_UNARY_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/UnaryExpressionType.hpp"
#include <memory>

namespace vnlc {
    class UnaryExpressionNode : public ExpressionNode {
    private:
        UnaryExpressionNode() = delete;

        UnaryExpressionType type;
        std::unique_ptr<ExpressionNode> operand;

    public:
        UnaryExpressionNode(UnaryExpressionType type, std::unique_ptr<ExpressionNode>&& operand, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const UnaryExpressionType getType() const noexcept;
        [[nodiscard]] const ExpressionNode& getOperand() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_UNARY_EXPRESSION_NODE_HPP