#ifndef VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/FunctionDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct NativeFunctionDeclarationParsingContext {
        FunctionDeclarationKind::Context context;
        FunctionDeclarationKind::AccessModifier accessModifier;
        FunctionDeclarationKind::Binding binding;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_NATIVE_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP