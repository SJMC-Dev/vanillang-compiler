#ifndef VNLC_EXPRESSION_STATEMENT_PARSING_RESULT_HPP
#define VNLC_EXPRESSION_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcExpressionStatementNode.hpp"
#include <memory>

struct VnlcExpressionStatementParsingResult {
    std::unique_ptr<VnlcExpressionStatementNode> statement;
};

#endif // VNLC_EXPRESSION_STATEMENT_PARSING_RESULT_HPP
