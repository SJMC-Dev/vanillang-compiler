#ifndef VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_CONTEXT_HPP
#define VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_CONTEXT_HPP

#include "ast/declaration/ValueDeclarationKind.hpp"

namespace vnlc {
    struct VariableDeclarationPrimaryParsingContext {
        ValueDeclarationKind::Kind kind;
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_CONTEXT_HPP
