#ifndef VNLC_SWITCH_STATEMENT_PARSING_RESULT_HPP
#define VNLC_SWITCH_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcSwitchStatementNode.hpp"
#include <memory>

struct VnlcSwitchStatementParsingResult {
    std::unique_ptr<VnlcSwitchStatementNode> statement;
};

#endif // VNLC_SWITCH_STATEMENT_PARSING_RESULT_HPP
