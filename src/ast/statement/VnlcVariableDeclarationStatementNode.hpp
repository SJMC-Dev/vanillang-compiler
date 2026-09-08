#ifndef VNLC_VARIABLE_DECLARATION_STATEMENT_NODE_HPP
#define VNLC_VARIABLE_DECLARATION_STATEMENT_NODE_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "ast/statement/VnlcStatementNode.hpp"
#include <memory>

class VnlcVariableDeclarationStatementNode : public VnlcStatementNode {
private:
    VnlcVariableDeclarationStatementNode() = delete;

    std::unique_ptr<VnlcValueDeclarationNode> variableDeclaration;

public:
    VnlcVariableDeclarationStatementNode(std::unique_ptr<VnlcValueDeclarationNode>&& variableDeclaration, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const VnlcValueDeclarationNode& getVariableDeclaration() const noexcept;
};

#endif // VNLC_VARIABLE_DECLARATION_STATEMENT_NODE_HPP