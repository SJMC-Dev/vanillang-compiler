#ifndef VNLC_BLOCK_STATEMENT_PARSING_RESULT_HPP
#define VNLC_BLOCK_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcBlockStatementNode.hpp"
#include <memory>

struct VnlcBlockStatementParsingResult {
    std::unique_ptr<VnlcBlockStatementNode> statement;
};

#endif // VNLC_BLOCK_STATEMENT_PARSING_RESULT_HPP
