#ifndef VNLC_CONTINUE_STATEMENT_PARSING_RESULT_HPP
#define VNLC_CONTINUE_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcContinueStatementNode.hpp"
#include <memory>

struct VnlcContinueStatementParsingResult {
    std::unique_ptr<VnlcContinueStatementNode> statement;
};

#endif // VNLC_CONTINUE_STATEMENT_PARSING_RESULT_HPP
