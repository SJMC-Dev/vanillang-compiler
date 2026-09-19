#include "SimpleLiteralExpressionNode.hpp"

namespace vnlc {
    SimpleLiteralExpressionNode::SimpleLiteralExpressionNode(SimpleLiteralExpressionType type, std::string_view literal, const Token& firstToken, const Token& lastToken) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          type(type),
          literal(std::string(literal)) {}

    const SimpleLiteralExpressionType SimpleLiteralExpressionNode::getType() const noexcept {
        return type;
    }

    std::string_view SimpleLiteralExpressionNode::getLiteral() const noexcept {
        return literal;
    }
} // namespace vnlc
