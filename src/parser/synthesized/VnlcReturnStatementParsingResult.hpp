#ifndef VNLC_RETURN_STATEMENT_PARSING_RESULT_HPP
#define VNLC_RETURN_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcReturnStatementNode.hpp"
#include <memory>

struct VnlcReturnStatementParsingResult {
    std::unique_ptr<VnlcReturnStatementNode> statement;
};

#endif // VNLC_RETURN_STATEMENT_PARSING_RESULT_HPP
