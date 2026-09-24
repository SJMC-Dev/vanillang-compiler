#include "SelectorLiteralExpressionNode.hpp"

namespace vnlc {
    SelectorLiteralExpressionNode::SelectorLiteralExpressionNode(
        SelectorLiteralExpressionKind kind,
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&& arguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          arguments(std::move(arguments)) {}

    const SelectorLiteralExpressionKind SelectorLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& SelectorLiteralExpressionNode::getArguments() const noexcept {
        return arguments;
    }
} // namespace vnlc
