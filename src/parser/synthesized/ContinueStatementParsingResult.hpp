#ifndef VNLC_CONTINUE_STATEMENT_PARSING_RESULT_HPP
#define VNLC_CONTINUE_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/ContinueStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ContinueStatementParsingResult {
        std::unique_ptr<ContinueStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_CONTINUE_STATEMENT_PARSING_RESULT_HPP
