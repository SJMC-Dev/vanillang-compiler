#ifndef VNLC_RELOAD_STATEMENT_PARSING_RESULT_HPP
#define VNLC_RELOAD_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcReloadStatementNode.hpp"
#include <memory>

struct VnlcReloadStatementParsingResult {
    std::unique_ptr<VnlcReloadStatementNode> statement;
};

#endif // VNLC_RELOAD_STATEMENT_PARSING_RESULT_HPP
