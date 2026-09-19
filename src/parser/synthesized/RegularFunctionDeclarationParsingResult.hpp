#ifndef VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_RESULT_HPP
#define VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/FunctionDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct RegularFunctionDeclarationParsingResult {
        std::unique_ptr<FunctionDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_RESULT_HPP
