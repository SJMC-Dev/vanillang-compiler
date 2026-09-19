#ifndef VNLC_VARIABLE_DECLARATION_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct VariableDeclarationParsingResult {
        std::unique_ptr<ValueDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_PARSING_RESULT_HPP
