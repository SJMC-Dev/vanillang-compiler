#ifndef VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP
#define VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcConstructorParsingContext {
    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_CONSTRUCTOR_PARSING_CONTEXT_HPP