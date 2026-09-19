#ifndef VNLC_FOR_STATEMENT_PARSING_RESULT_HPP
#define VNLC_FOR_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/ForStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ForStatementParsingResult {
        std::unique_ptr<ForStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_FOR_STATEMENT_PARSING_RESULT_HPP
