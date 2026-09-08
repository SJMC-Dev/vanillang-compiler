#ifndef VNLC_TYPE_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_TYPE_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcTypeDeclarationParsingContext {
    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_TYPE_DECLARATION_PARSING_CONTEXT_HPP