#ifndef VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include "ast/declaration/VnlcPropertyDeclarationType.hpp"
#include <vector>

struct VnlcPropertyDeclarationParsingContext {
    VnlcPropertyDeclarationType::AccessModifier accessModifier;
    VnlcPropertyDeclarationType::Binding binding;

    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP