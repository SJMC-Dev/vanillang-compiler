#ifndef VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include "ast/declaration/VnlcFunctionDeclarationType.hpp"
#include <vector>

struct VnlcRegularFunctionDeclarationParsingContext {
    VnlcFunctionDeclarationType::Context context;
    VnlcFunctionDeclarationType::AccessModifier accessModifier;
    VnlcFunctionDeclarationType::Binding binding;

    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_REGULAR_FUNCTION_DECLARATION_PARSING_CONTEXT_HPP