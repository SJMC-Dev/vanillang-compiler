#include "ForStatementNode.hpp"

namespace vnlc {
    ForStatementNode::ForStatementNode(
        std::unique_ptr<ValueDeclarationNode>&& loopVariable,
        std::unique_ptr<ExpressionNode>&& iterableExpression,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(std::nullopt, firstToken, lastToken),
          loopVariable(std::move(loopVariable)),
          iterableExpression(std::move(iterableExpression)),
          body(std::move(body)) {}

    ForStatementNode::ForStatementNode(
        std::unique_ptr<IdentifierNode>&& label,
        std::unique_ptr<ValueDeclarationNode>&& loopVariable,
        std::unique_ptr<ExpressionNode>&& iterableExpression,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(std::make_optional(std::move(label)), firstToken, lastToken),
          loopVariable(std::move(loopVariable)),
          iterableExpression(std::move(iterableExpression)),
          body(std::move(body)) {}

    const ValueDeclarationNode& ForStatementNode::getLoopVariable() const noexcept {
        return *loopVariable;
    }

    const ExpressionNode& ForStatementNode::getIterableExpression() const noexcept {
        return *iterableExpression;
    }

    const StatementNode& ForStatementNode::getBody() const noexcept {
        return *body;
    }
} // namespace vnlc
