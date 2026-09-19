#ifndef VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/FunctionDeclarationType.hpp"
#include <vector>

namespace vnlc {
    struct RegularFunctionDeclarationParsingContext {
        FunctionDeclarationType::Context context;
        FunctionDeclarationType::AccessModifier accessModifier;
        FunctionDeclarationType::Binding binding;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP