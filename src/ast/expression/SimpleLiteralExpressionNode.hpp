#ifndef VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/LiteralExpressionNode.hpp"
#include "ast/expression/SimpleLiteralExpressionType.hpp"

namespace vnlc {
    class SimpleLiteralExpressionNode : public LiteralExpressionNode {
    private:
        SimpleLiteralExpressionNode() = delete;

        SimpleLiteralExpressionType type;
        std::string literal;

    public:
        SimpleLiteralExpressionNode(SimpleLiteralExpressionType type, std::string_view literal, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const SimpleLiteralExpressionType getType() const noexcept;
        [[nodiscard]] std::string_view getLiteral() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP