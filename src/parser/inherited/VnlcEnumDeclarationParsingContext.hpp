#ifndef VNLC_ENUM_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_ENUM_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcEnumDeclarationParsingContext {
    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_ENUM_DECLARATION_PARSING_CONTEXT_HPP