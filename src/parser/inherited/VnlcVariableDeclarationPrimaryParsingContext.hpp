#ifndef VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_CONTEXT_HPP
#define VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcValueDeclarationType.hpp"

struct VnlcVariableDeclarationPrimaryParsingContext {
    VnlcValueDeclarationType::Kind kind;
};

#endif // VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_CONTEXT_HPP
