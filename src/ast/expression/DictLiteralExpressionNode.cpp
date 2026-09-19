#include "DictLiteralExpressionNode.hpp"

namespace vnlc {
    DictLiteralExpressionNode::DictLiteralExpressionNode(std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&& entries, const Token& firstToken, const Token& lastToken) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          entries(std::move(entries)) {}

    const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& DictLiteralExpressionNode::getEntries() const noexcept {
        return entries;
    }
} // namespace vnlc
