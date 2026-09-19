#include "StringLiteralExpressionNode.hpp"

namespace vnlc {
    StringLiteralExpressionNode::StringLiteralExpressionNode(
        StringLiteralExpressionType type,
        std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>&& parts,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          type(type),
          parts(std::move(parts)) {}

    const StringLiteralExpressionType StringLiteralExpressionNode::getType() const noexcept {
        return type;
    }

    const std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>& StringLiteralExpressionNode::getParts() const noexcept {
        return parts;
    }
} // namespace vnlc
