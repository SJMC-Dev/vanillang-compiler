#ifndef VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP
#define VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct ConstructorParsingContext {
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP