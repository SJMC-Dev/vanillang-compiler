#ifndef VNLC_RETURN_STATEMENT_PARSING_RESULT_HPP
#define VNLC_RETURN_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/ReturnStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ReturnStatementParsingResult {
        std::unique_ptr<ReturnStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_RETURN_STATEMENT_PARSING_RESULT_HPP
