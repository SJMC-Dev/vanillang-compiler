#ifndef VNLC_SUBSCRIPT_EXPRESSION_NODE_HPP
#define VNLC_SUBSCRIPT_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class SubscriptExpressionNode : public ExpressionNode {
    private:
        SubscriptExpressionNode() = delete;

        std::unique_ptr<ExpressionNode> object;
        std::unique_ptr<ExpressionNode> index;

    public:
        SubscriptExpressionNode(std::unique_ptr<ExpressionNode>&& object, std::unique_ptr<ExpressionNode>&& index, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode& getObject() const noexcept;
        [[nodiscard]] const ExpressionNode& getIndex() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SUBSCRIPT_EXPRESSION_NODE_HPP