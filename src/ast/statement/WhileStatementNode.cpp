#include "WhileStatementNode.hpp"

namespace vnlc {
    WhileStatementNode::WhileStatementNode(std::unique_ptr<ExpressionNode>&& condition, std::unique_ptr<StatementNode>&& body, const Token& firstToken, const Token& lastToken) noexcept
        : LoopStatementNode(std::nullopt, firstToken, lastToken),
          condition(std::move(condition)),
          body(std::move(body)) {}

    WhileStatementNode::WhileStatementNode(
        std::unique_ptr<IdentifierNode>&& label,
        std::unique_ptr<ExpressionNode>&& condition,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(std::make_optional(std::move(label)), firstToken, lastToken),
          condition(std::move(condition)),
          body(std::move(body)) {}

    const ExpressionNode& WhileStatementNode::getCondition() const noexcept {
        return *condition;
    }

    const StatementNode& WhileStatementNode::getBody() const noexcept {
        return *body;
    }
} // namespace vnlc
