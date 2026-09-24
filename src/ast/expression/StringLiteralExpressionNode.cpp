#include "StringLiteralExpressionNode.hpp"

namespace vnlc {
    StringLiteralExpressionNode::StringLiteralExpressionNode(
        StringLiteralExpressionKind kind,
        std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>&& parts,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          parts(std::move(parts)) {}

    const StringLiteralExpressionKind StringLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    const std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>& StringLiteralExpressionNode::getParts() const noexcept {
        return parts;
    }
} // namespace vnlc
