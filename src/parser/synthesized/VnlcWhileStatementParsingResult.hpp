#ifndef VNLC_WHILE_STATEMENT_PARSING_RESULT_HPP
#define VNLC_WHILE_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcWhileStatementNode.hpp"
#include <memory>

struct VnlcWhileStatementParsingResult {
    std::unique_ptr<VnlcWhileStatementNode> statement;
};

#endif // VNLC_WHILE_STATEMENT_PARSING_RESULT_HPP
