#ifndef VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/ValueDeclarationType.hpp"
#include <vector>

namespace vnlc {
    struct PropertyDeclarationParsingContext {
        ValueDeclarationType::Kind kind;
        ValueDeclarationType::AccessModifier accessModifier;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP
