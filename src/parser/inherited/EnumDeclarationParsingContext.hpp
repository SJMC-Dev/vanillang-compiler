#ifndef VNLC_ENUM_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_ENUM_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct EnumDeclarationParsingContext {
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_ENUM_DECLARATION_PARSING_CONTEXT_HPP