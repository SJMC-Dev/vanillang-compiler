#include "SimpleLiteralExpressionNode.hpp"

namespace vnlc {
    SimpleLiteralExpressionNode::SimpleLiteralExpressionNode(SimpleLiteralExpressionKind kind, std::string_view literal, const Token& firstToken, const Token& lastToken) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          literal(std::string(literal)) {}

    const SimpleLiteralExpressionKind SimpleLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    std::string_view SimpleLiteralExpressionNode::getLiteral() const noexcept {
        return literal;
    }
} // namespace vnlc
