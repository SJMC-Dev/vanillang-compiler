#ifndef VNLC_WHILE_STATEMENT_NODE_HPP
#define VNLC_WHILE_STATEMENT_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/LoopStatementNode.hpp"
#include <memory>

namespace vnlc {
    class WhileStatementNode : public LoopStatementNode {
    private:
        WhileStatementNode() = delete;

        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<StatementNode> body;

    public:
        WhileStatementNode(std::unique_ptr<ExpressionNode>&& condition, std::unique_ptr<StatementNode>&& body, const Token& firstToken, const Token& lastToken) noexcept;

        WhileStatementNode(
            std::unique_ptr<IdentifierNode>&& label,
            std::unique_ptr<ExpressionNode>&& condition,
            std::unique_ptr<StatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getCondition() const noexcept;
        [[nodiscard]] const StatementNode& getBody() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_WHILE_STATEMENT_NODE_HPP