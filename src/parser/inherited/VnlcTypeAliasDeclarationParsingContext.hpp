#ifndef VNLC_TYPE_ALIAS_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcTypeAliasDeclarationParsingContext {
    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_TYPE_ALIAS_DECLARATION_PARSING_CONTEXT_HPP