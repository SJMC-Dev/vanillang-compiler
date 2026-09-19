#ifndef VNLC_RELOAD_STATEMENT_NODE_HPP
#define VNLC_RELOAD_STATEMENT_NODE_HPP

#include "ast/statement/ControlFlowStatementNode.hpp"

namespace vnlc {
    class ReloadStatementNode : public ControlFlowStatementNode {
    private:
        ReloadStatementNode() = delete;

    public:
        ReloadStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_RELOAD_STATEMENT_NODE_HPP