#ifndef VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP
#define VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP

#include "ast/statement/StatementNode.hpp"

namespace vnlc {
    class ControlFlowStatementNode : public StatementNode {
    private:
        ControlFlowStatementNode() = delete;

    protected:
        ControlFlowStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP