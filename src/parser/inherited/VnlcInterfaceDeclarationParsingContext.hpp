#ifndef VNLC_INTERFACE_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_INTERFACE_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcInterfaceDeclarationParsingContext {
    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_INTERFACE_DECLARATION_PARSING_CONTEXT_HPP