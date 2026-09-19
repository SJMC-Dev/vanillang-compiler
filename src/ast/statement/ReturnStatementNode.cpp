#include "ReturnStatementNode.hpp"

namespace vnlc {
    ReturnStatementNode::ReturnStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), returnValue(std::nullopt) {}

    ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ExpressionNode>&& returnValue, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          returnValue(std::make_optional(std::move(returnValue))) {}

    const std::optional<std::unique_ptr<ExpressionNode>>& ReturnStatementNode::getReturnValue() const noexcept {
        return returnValue;
    }
} // namespace vnlc
