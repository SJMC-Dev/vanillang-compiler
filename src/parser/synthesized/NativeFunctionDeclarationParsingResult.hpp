#ifndef VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_RESULT_HPP
#define VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/FunctionDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct NativeFunctionDeclarationParsingResult {
        std::unique_ptr<FunctionDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_RESULT_HPP
