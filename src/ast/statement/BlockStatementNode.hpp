#ifndef VNLC_BLOCK_STATEMENT_NODE_HPP
#define VNLC_BLOCK_STATEMENT_NODE_HPP

#include "ast/statement/StatementNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class BlockStatementNode : public StatementNode {
    private:
        BlockStatementNode() = delete;

        std::vector<std::unique_ptr<StatementNode>> statements;

    public:
        BlockStatementNode(std::vector<std::unique_ptr<StatementNode>>&& statements, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const std::vector<std::unique_ptr<StatementNode>>& getStatements() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_BLOCK_STATEMENT_NODE_HPP
