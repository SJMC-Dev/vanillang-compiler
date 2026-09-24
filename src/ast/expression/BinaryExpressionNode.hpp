#ifndef VNLC_BINARY_EXPRESSION_NODE_HPP
#define VNLC_BINARY_EXPRESSION_NODE_HPP

#include "ast/expression/BinaryExpressionKind.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class BinaryExpressionNode : public ExpressionNode {
    private:
        BinaryExpressionNode() = delete;

        BinaryExpressionKind kind;
        std::unique_ptr<ExpressionNode> left;
        std::unique_ptr<ExpressionNode> right;

    public:
        BinaryExpressionNode(
            BinaryExpressionKind kind,
            std::unique_ptr<ExpressionNode>&& left,
            std::unique_ptr<ExpressionNode>&& right,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const BinaryExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getLeft() const noexcept;
        [[nodiscard]] const ExpressionNode& getRight() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_BINARY_EXPRESSION_NODE_HPP
