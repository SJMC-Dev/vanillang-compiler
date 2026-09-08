#ifndef VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP
#define VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP

#include "ast/statement/VnlcStatementNode.hpp"

class VnlcControlFlowStatementNode : public VnlcStatementNode {
private:
    VnlcControlFlowStatementNode() = delete;

protected:
    VnlcControlFlowStatementNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
};

#endif // VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP