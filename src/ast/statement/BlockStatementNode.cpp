#include "BlockStatementNode.hpp"

namespace vnlc {
    BlockStatementNode::BlockStatementNode(std::vector<std::unique_ptr<StatementNode>>&& statements, const Token& firstToken, const Token& lastToken) noexcept
        : StatementNode(firstToken, lastToken),
          statements(std::move(statements)) {}

    const std::vector<std::unique_ptr<StatementNode>>& BlockStatementNode::getStatements() const noexcept {
        return statements;
    }
} // namespace vnlc
