#include "SelectorLiteralExpressionNode.hpp"

namespace vnlc {
    SelectorLiteralExpressionNode::SelectorLiteralExpressionNode(
        SelectorLiteralExpressionType type,
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&& arguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          type(type),
          arguments(std::move(arguments)) {}

    const SelectorLiteralExpressionType SelectorLiteralExpressionNode::getType() const noexcept {
        return type;
    }

    const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& SelectorLiteralExpressionNode::getArguments() const noexcept {
        return arguments;
    }
} // namespace vnlc
