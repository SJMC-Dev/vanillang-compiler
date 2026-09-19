#ifndef VNLC_STATEMENT_NODE_HPP
#define VNLC_STATEMENT_NODE_HPP

#include "ast/AstNode.hpp"

namespace vnlc {
    class StatementNode : public AstNode {
    private:
        StatementNode() = delete;

    protected:
        StatementNode(const Token& firstToken, const Token& lastToken);
    };
} // namespace vnlc

#endif // VNLC_STATEMENT_NODE_HPP