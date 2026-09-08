#ifndef VNLC_STATEMENT_NODE_HPP
#define VNLC_STATEMENT_NODE_HPP

#include "ast/VnlcAstNode.hpp"

class VnlcStatementNode : public VnlcAstNode {
private:
    VnlcStatementNode() = delete;

protected:
    VnlcStatementNode(const VnlcToken& firstToken, const VnlcToken& lastToken);
};

#endif // VNLC_STATEMENT_NODE_HPP