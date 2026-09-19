#ifndef VNLC_VARIABLE_DECLARATION_STATEMENT_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_STATEMENT_PARSING_RESULT_HPP

#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct VariableDeclarationStatementParsingResult {
        std::unique_ptr<VariableDeclarationStatementNode> statement;
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_STATEMENT_PARSING_RESULT_HPP
