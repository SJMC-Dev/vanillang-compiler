#ifndef VNLC_FUNCTION_DECLARATION_PARSING_RESULT_HPP
#define VNLC_FUNCTION_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include <memory>

struct VnlcFunctionDeclarationParsingResult {
    std::unique_ptr<VnlcFunctionDeclarationNode> declaration;
};

#endif // VNLC_FUNCTION_DECLARATION_PARSING_RESULT_HPP
