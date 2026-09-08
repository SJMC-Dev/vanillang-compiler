#ifndef VNLC_FOR_STATEMENT_NODE_HPP
#define VNLC_FOR_STATEMENT_NODE_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/statement/VnlcLoopStatementNode.hpp"
#include <memory>

class VnlcForStatementNode : public VnlcLoopStatementNode {
private:
    VnlcForStatementNode() = delete;

    std::unique_ptr<VnlcValueDeclarationNode> loopVariable;
    std::unique_ptr<VnlcExpressionNode> iterableExpression;
    std::unique_ptr<VnlcStatementNode> body;

public:
    VnlcForStatementNode(
        std::unique_ptr<VnlcValueDeclarationNode>&& loopVariable,
        std::unique_ptr<VnlcExpressionNode>&& iterableExpression,
        std::unique_ptr<VnlcStatementNode>&& body,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcForStatementNode(
        std::unique_ptr<VnlcIdentifierNode>&& label,
        std::unique_ptr<VnlcValueDeclarationNode>&& loopVariable,
        std::unique_ptr<VnlcExpressionNode>&& iterableExpression,
        std::unique_ptr<VnlcStatementNode>&& body,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcValueDeclarationNode& getLoopVariable() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getIterableExpression() const noexcept;
    [[nodiscard]] const VnlcStatementNode& getBody() const noexcept;
};

#endif // VNLC_FOR_STATEMENT_NODE_HPP