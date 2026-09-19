#include "VariableDeclarationStatementNode.hpp"

namespace vnlc {
    VariableDeclarationStatementNode::VariableDeclarationStatementNode(std::unique_ptr<ValueDeclarationNode>&& variableDeclaration, const Token& firstToken, const Token& lastToken) noexcept
        : StatementNode(firstToken, lastToken),
          variableDeclaration(std::move(variableDeclaration)) {}

    const ValueDeclarationNode& VariableDeclarationStatementNode::getVariableDeclaration() const noexcept {
        return *variableDeclaration;
    }
} // namespace vnlc
