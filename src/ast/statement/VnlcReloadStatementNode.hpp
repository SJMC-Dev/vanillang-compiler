#ifndef VNLC_RELOAD_STATEMENT_NODE_HPP
#define VNLC_RELOAD_STATEMENT_NODE_HPP

#include "ast/statement/VnlcControlFlowStatementNode.hpp"

class VnlcReloadStatementNode : public VnlcControlFlowStatementNode {
private:
    VnlcReloadStatementNode() = delete;

public:
    VnlcReloadStatementNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
};

#endif // VNLC_RELOAD_STATEMENT_NODE_HPP