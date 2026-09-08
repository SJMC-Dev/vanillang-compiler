#ifndef VNLC_IF_STATEMENT_PARSING_RESULT_HPP
#define VNLC_IF_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcIfStatementNode.hpp"
#include <memory>

struct VnlcIfStatementParsingResult {
    std::unique_ptr<VnlcIfStatementNode> statement;
};

#endif // VNLC_IF_STATEMENT_PARSING_RESULT_HPP
