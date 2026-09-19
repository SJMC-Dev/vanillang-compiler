#ifndef VNLC_EXPRESSION_STATEMENT_PARSING_RESULT_HPP
#define VNLC_EXPRESSION_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/ExpressionStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ExpressionStatementParsingResult {
        std::unique_ptr<ExpressionStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_EXPRESSION_STATEMENT_PARSING_RESULT_HPP
