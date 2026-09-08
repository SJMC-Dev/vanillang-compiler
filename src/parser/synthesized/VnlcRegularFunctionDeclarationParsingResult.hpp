#ifndef VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_RESULT_HPP
#define VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include <memory>

struct VnlcRegularFunctionDeclarationParsingResult {
    std::unique_ptr<VnlcFunctionDeclarationNode> declaration;
};

#endif // VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_RESULT_HPP
