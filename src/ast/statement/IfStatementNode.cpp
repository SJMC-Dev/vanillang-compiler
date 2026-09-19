#include "IfStatementNode.hpp"

namespace vnlc {
    IfStatementNode::IfStatementNode(
        std::unique_ptr<ExpressionNode>&& condition,
        std::unique_ptr<StatementNode>&& thenBranch,
        std::optional<std::unique_ptr<StatementNode>>&& elseBranch,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    const ExpressionNode& IfStatementNode::getCondition() const noexcept {
        return *condition;
    }

    const StatementNode& IfStatementNode::getThenBranch() const noexcept {
        return *thenBranch;
    }

    const std::optional<std::unique_ptr<StatementNode>>& IfStatementNode::getElseBranch() const noexcept {
        return elseBranch;
    }
} // namespace vnlc
