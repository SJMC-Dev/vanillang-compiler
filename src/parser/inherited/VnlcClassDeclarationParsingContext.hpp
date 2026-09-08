#ifndef VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcClassDeclarationParsingContext {
    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP