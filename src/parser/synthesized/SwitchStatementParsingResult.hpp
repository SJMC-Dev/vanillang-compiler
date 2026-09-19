#ifndef VNLC_SWITCH_STATEMENT_PARSING_RESULT_HPP
#define VNLC_SWITCH_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/SwitchStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct SwitchStatementParsingResult {
        std::unique_ptr<SwitchStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_SWITCH_STATEMENT_PARSING_RESULT_HPP
