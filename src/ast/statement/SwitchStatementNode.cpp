#include "SwitchStatementNode.hpp"

namespace vnlc {
    SwitchStatementNode::SwitchStatementNode(
        std::unique_ptr<ExpressionNode>&& switchExpression,
        std::vector<SwitchStatementItem::LiteralMatchItem>&& literalMatchItems,
        std::optional<std::unique_ptr<StatementNode>>&& defaultCaseBody,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          switchExpression(std::move(switchExpression)),
          switchKind(SwitchStatementKind::LITERAL_MATCH),
          literalMatchItems(std::move(literalMatchItems)),
          typeMatchItems(),
          defaultCaseBody(std::move(defaultCaseBody)) {}

    SwitchStatementNode::SwitchStatementNode(
        std::unique_ptr<ExpressionNode>&& switchExpression,
        std::vector<SwitchStatementItem::TypeMatchItem>&& typeMatchItems,
        std::optional<std::unique_ptr<StatementNode>>&& defaultCaseBody,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          switchExpression(std::move(switchExpression)),
          switchKind(SwitchStatementKind::TYPE_MATCH),
          literalMatchItems(),
          typeMatchItems(std::move(typeMatchItems)),
          defaultCaseBody(std::move(defaultCaseBody)) {}

    const ExpressionNode& SwitchStatementNode::getSwitchExpression() const noexcept {
        return *switchExpression;
    }

    const SwitchStatementKind SwitchStatementNode::getSwitchKind() const noexcept {
        return switchKind;
    }

    const std::vector<SwitchStatementItem::LiteralMatchItem>& SwitchStatementNode::getLiteralMatchItems() const noexcept {
        return literalMatchItems;
    }

    const std::vector<SwitchStatementItem::TypeMatchItem>& SwitchStatementNode::getTypeMatchItems() const noexcept {
        return typeMatchItems;
    }

    const std::optional<std::unique_ptr<StatementNode>>& SwitchStatementNode::getDefaultCaseBody() const noexcept {
        return defaultCaseBody;
    }
} // namespace vnlc
