#ifndef VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcLiteralExpressionNode.hpp"
#include "ast/expression/VnlcSimpleLiteralExpressionType.hpp"

class VnlcSimpleLiteralExpressionNode : public VnlcLiteralExpressionNode {
private:
    VnlcSimpleLiteralExpressionNode() = delete;

    VnlcSimpleLiteralExpressionType type;
    std::string literal;

public:
    VnlcSimpleLiteralExpressionNode(VnlcSimpleLiteralExpressionType type, std::string_view literal, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const VnlcSimpleLiteralExpressionType getType() const noexcept;
    [[nodiscard]] std::string_view getLiteral() const noexcept;
};

#endif // VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP