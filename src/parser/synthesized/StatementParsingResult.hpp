#ifndef VNLC_STATEMENT_PARSING_RESULT_HPP
#define VNLC_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/StatementNode.hpp"
#include <memory>

namespace vnlc {
    struct StatementParsingResult {
        std::unique_ptr<StatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_STATEMENT_PARSING_RESULT_HPP
