#ifndef VNLC_IF_STATEMENT_NODE_HPP
#define VNLC_IF_STATEMENT_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    class IfStatementNode : public ControlFlowStatementNode {
    private:
        IfStatementNode() = delete;

        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<StatementNode> thenBranch;
        std::optional<std::unique_ptr<StatementNode>> elseBranch; // nullopt if no else branch

    public:
        IfStatementNode(
            std::unique_ptr<ExpressionNode>&& condition,
            std::unique_ptr<StatementNode>&& thenBranch,
            std::optional<std::unique_ptr<StatementNode>>&& elseBranch,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getCondition() const noexcept;
        [[nodiscard]] const StatementNode& getThenBranch() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<StatementNode>>& getElseBranch() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_IF_STATEMENT_NODE_HPP