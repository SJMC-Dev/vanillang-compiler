#ifndef VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_RESULT_HPP
#define VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include <memory>

struct VnlcNativeFunctionDeclarationParsingResult {
    std::unique_ptr<VnlcFunctionDeclarationNode> declaration;
};

#endif // VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_RESULT_HPP
