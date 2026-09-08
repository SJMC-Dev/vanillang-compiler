#ifndef VNLC_IF_STATEMENT_NODE_HPP
#define VNLC_IF_STATEMENT_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/statement/VnlcControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

class VnlcIfStatementNode : public VnlcControlFlowStatementNode {
private:
    VnlcIfStatementNode() = delete;

    std::unique_ptr<VnlcExpressionNode> condition;
    std::unique_ptr<VnlcStatementNode> thenBranch;
    std::optional<std::unique_ptr<VnlcStatementNode>> elseBranch; // nullopt if no else branch

public:
    VnlcIfStatementNode(
        std::unique_ptr<VnlcExpressionNode>&& condition,
        std::unique_ptr<VnlcStatementNode>&& thenBranch,
        std::optional<std::unique_ptr<VnlcStatementNode>>&& elseBranch,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcExpressionNode& getCondition() const noexcept;
    [[nodiscard]] const VnlcStatementNode& getThenBranch() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcStatementNode>>& getElseBranch() const noexcept;
};

#endif // VNLC_IF_STATEMENT_NODE_HPP