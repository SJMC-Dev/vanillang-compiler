#include "IdentifierLikeExpressionNode.hpp"

namespace vnlc {
    IdentifierLikeExpressionNode::IdentifierLikeExpressionNode(std::unique_ptr<IdentifierNode>&& name, const Token& firstToken, const Token& lastToken) noexcept
        : PrimaryExpressionNode(firstToken, lastToken),
          name(std::move(name)) {}

    IdentifierLikeExpressionNode::IdentifierLikeExpressionNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierLikeExpressionNode>>&& genericArguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : PrimaryExpressionNode(firstToken, lastToken),
          name(std::move(name)),
          genericArguments(std::move(genericArguments)) {}

    const IdentifierNode& IdentifierLikeExpressionNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<IdentifierLikeExpressionNode>>& IdentifierLikeExpressionNode::getGenericArguments() const noexcept {
        return genericArguments;
    }
} // namespace vnlc
