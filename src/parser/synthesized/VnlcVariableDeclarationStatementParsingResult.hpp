#ifndef VNLC_VARIABLE_DECLARATION_STATEMENT_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VnlcVariableDeclarationStatementNode.hpp"
#include <memory>

struct VnlcVariableDeclarationStatementParsingResult {
    std::unique_ptr<VnlcVariableDeclarationStatementNode> statement;
};

#endif // VNLC_VARIABLE_DECLARATION_STATEMENT_PARSING_RESULT_HPP
