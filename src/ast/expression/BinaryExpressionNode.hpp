#ifndef VNLC_BINARY_EXPRESSION_NODE_HPP
#define VNLC_BINARY_EXPRESSION_NODE_HPP

#include "ast/expression/BinaryExpressionType.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class BinaryExpressionNode : public ExpressionNode {
    private:
        BinaryExpressionNode() = delete;

        BinaryExpressionType type;
        std::unique_ptr<ExpressionNode> left;
        std::unique_ptr<ExpressionNode> right;

    public:
        BinaryExpressionNode(
            BinaryExpressionType type,
            std::unique_ptr<ExpressionNode>&& left,
            std::unique_ptr<ExpressionNode>&& right,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const BinaryExpressionType getType() const noexcept;
        [[nodiscard]] const ExpressionNode& getLeft() const noexcept;
        [[nodiscard]] const ExpressionNode& getRight() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_BINARY_EXPRESSION_NODE_HPP
