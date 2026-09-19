#ifndef VNLC_BREAK_STATEMENT_PARSING_RESULT_HPP
#define VNLC_BREAK_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/BreakStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct BreakStatementParsingResult {
        std::unique_ptr<BreakStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_BREAK_STATEMENT_PARSING_RESULT_HPP
