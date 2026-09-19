#ifndef VNLC_WHILE_STATEMENT_PARSING_RESULT_HPP
#define VNLC_WHILE_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/WhileStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct WhileStatementParsingResult {
        std::unique_ptr<WhileStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_WHILE_STATEMENT_PARSING_RESULT_HPP
