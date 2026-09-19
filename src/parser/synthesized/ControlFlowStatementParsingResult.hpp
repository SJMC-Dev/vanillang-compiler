#ifndef VNLC_CONTROL_FLOW_STATEMENT_PARSING_RESULT_HPP
#define VNLC_CONTROL_FLOW_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ControlFlowStatementParsingResult {
        std::unique_ptr<ControlFlowStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_CONTROL_FLOW_STATEMENT_PARSING_RESULT_HPP
