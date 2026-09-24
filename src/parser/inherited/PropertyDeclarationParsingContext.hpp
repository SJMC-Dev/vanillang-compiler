#ifndef VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct PropertyDeclarationParsingContext {
        ValueDeclarationKind::Kind kind;
        ValueDeclarationKind::AccessModifier accessModifier;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_PROPERTY_DECLARATION_PARSING_CONTEXT_HPP
