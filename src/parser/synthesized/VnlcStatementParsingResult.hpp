#ifndef VNLC_STATEMENT_PARSING_RESULT_HPP
#define VNLC_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcStatementNode.hpp"
#include <memory>

struct VnlcStatementParsingResult {
    std::unique_ptr<VnlcStatementNode> statement;
};

#endif // VNLC_STATEMENT_PARSING_RESULT_HPP
