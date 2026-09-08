#ifndef VNLC_EXPRESSION_STATEMENT_NODE_HPP
#define VNLC_EXPRESSION_STATEMENT_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/statement/VnlcStatementNode.hpp"
#include <memory>

class VnlcExpressionStatementNode : public VnlcStatementNode {
private:
    VnlcExpressionStatementNode() = delete;

    std::unique_ptr<VnlcExpressionNode> expression;

public:
    VnlcExpressionStatementNode(std::unique_ptr<VnlcExpressionNode>&& expression, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const VnlcExpressionNode& getExpression() const noexcept;
};

#endif // VNLC_EXPRESSION_STATEMENT_NODE_HPP
