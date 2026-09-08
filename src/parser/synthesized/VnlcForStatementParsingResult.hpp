#ifndef VNLC_FOR_STATEMENT_PARSING_RESULT_HPP
#define VNLC_FOR_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcForStatementNode.hpp"
#include <memory>

struct VnlcForStatementParsingResult {
    std::unique_ptr<VnlcForStatementNode> statement;
};

#endif // VNLC_FOR_STATEMENT_PARSING_RESULT_HPP
