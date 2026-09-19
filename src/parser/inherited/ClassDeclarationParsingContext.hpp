#ifndef VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct ClassDeclarationParsingContext {
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP