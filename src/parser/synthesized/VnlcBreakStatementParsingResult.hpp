#ifndef VNLC_BREAK_STATEMENT_PARSING_RESULT_HPP
#define VNLC_BREAK_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcBreakStatementNode.hpp"
#include <memory>

struct VnlcBreakStatementParsingResult {
    std::unique_ptr<VnlcBreakStatementNode> statement;
};

#endif // VNLC_BREAK_STATEMENT_PARSING_RESULT_HPP
