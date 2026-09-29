#ifndef VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP
#define VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/FunctionDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct ConstructorParsingContext {
        FunctionDeclarationKind::AccessModifier accessModifier;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP
