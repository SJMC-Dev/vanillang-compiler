#ifndef VNLC_IF_STATEMENT_PARSING_RESULT_HPP
#define VNLC_IF_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/IfStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct IfStatementParsingResult {
        std::unique_ptr<IfStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_IF_STATEMENT_PARSING_RESULT_HPP
