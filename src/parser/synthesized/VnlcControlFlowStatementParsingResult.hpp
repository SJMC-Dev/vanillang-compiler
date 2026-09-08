#ifndef VNLC_CONTROL_FLOW_STATEMENT_PARSING_RESULT_HPP
#define VNLC_CONTROL_FLOW_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcControlFlowStatementNode.hpp"
#include <memory>

struct VnlcControlFlowStatementParsingResult {
    std::unique_ptr<VnlcControlFlowStatementNode> statement;
};

#endif // VNLC_CONTROL_FLOW_STATEMENT_PARSING_RESULT_HPP
