#ifndef VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/PropertyDeclarationType.hpp"
#include <vector>

namespace vnlc {
    struct PropertyDeclarationParsingContext {
        PropertyDeclarationType::AccessModifier accessModifier;
        PropertyDeclarationType::Binding binding;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP