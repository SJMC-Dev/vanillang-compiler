#include "FunctionCallExpressionNode.hpp"

namespace vnlc {
    FunctionCallExpressionNode::FunctionCallExpressionNode(
        std::unique_ptr<ExpressionNode>&& callee,
        std::vector<std::unique_ptr<ExpressionNode>>&& arguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          callee(std::move(callee)),
          arguments(std::move(arguments)),
          context(std::nullopt) {}

    FunctionCallExpressionNode::FunctionCallExpressionNode(
        std::unique_ptr<ExpressionNode>&& callee,
        std::vector<std::unique_ptr<ExpressionNode>>&& arguments,
        std::unique_ptr<ExpressionNode>&& context,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          callee(std::move(callee)),
          arguments(std::move(arguments)),
          context(std::move(context)) {}

    const ExpressionNode& FunctionCallExpressionNode::getCallee() const noexcept {
        return *callee;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& FunctionCallExpressionNode::getArguments() const noexcept {
        return arguments;
    }

    const std::optional<std::unique_ptr<ExpressionNode>>& FunctionCallExpressionNode::getContext() const noexcept {
        return context;
    }
} // namespace vnlc
