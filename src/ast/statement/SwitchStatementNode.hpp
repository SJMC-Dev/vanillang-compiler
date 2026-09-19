#ifndef VNLC_SWITCH_STATEMENT_NODE_HPP
#define VNLC_SWITCH_STATEMENT_NODE_HPP

#include "ast/statement/ControlFlowStatementNode.hpp"
#include "ast/statement/SwitchStatementItem.hpp"
#include "ast/statement/SwitchStatementType.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class SwitchStatementNode : public ControlFlowStatementNode {
    private:
        SwitchStatementNode() = delete;

        std::unique_ptr<ExpressionNode> switchExpression;
        SwitchStatementType switchType;
        std::vector<SwitchStatementItem::LiteralMatchItem> literalMatchItems; // empty if switchType is TYPE_MATCH
        std::vector<SwitchStatementItem::TypeMatchItem> typeMatchItems;       // empty if switchType is LITERAL_MATCH
        std::optional<std::unique_ptr<StatementNode>> defaultCaseBody;

    public:
        SwitchStatementNode(
            std::unique_ptr<ExpressionNode>&& switchExpression,
            std::vector<SwitchStatementItem::LiteralMatchItem>&& literalMatchItems,
            std::optional<std::unique_ptr<StatementNode>>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        SwitchStatementNode(
            std::unique_ptr<ExpressionNode>&& switchExpression,
            std::vector<SwitchStatementItem::TypeMatchItem>&& typeMatchItems,
            std::optional<std::unique_ptr<StatementNode>>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getSwitchExpression() const noexcept;
        [[nodiscard]] const SwitchStatementType getSwitchType() const noexcept;
        [[nodiscard]] const std::vector<SwitchStatementItem::LiteralMatchItem>& getLiteralMatchItems() const noexcept;
        [[nodiscard]] const std::vector<SwitchStatementItem::TypeMatchItem>& getTypeMatchItems() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<StatementNode>>& getDefaultCaseBody() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SWITCH_STATEMENT_NODE_HPP