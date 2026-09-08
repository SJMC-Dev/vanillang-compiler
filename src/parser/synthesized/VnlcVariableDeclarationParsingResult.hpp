#ifndef VNLC_VARIABLE_DECLARATION_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include <memory>

struct VnlcVariableDeclarationParsingResult {
    std::unique_ptr<VnlcValueDeclarationNode> declaration;
};

#endif // VNLC_VARIABLE_DECLARATION_PARSING_RESULT_HPP
