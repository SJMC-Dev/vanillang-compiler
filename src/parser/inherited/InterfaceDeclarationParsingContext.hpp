#ifndef VNLC_INTERFACE_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_INTERFACE_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct InterfaceDeclarationParsingContext {
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_DECLARATION_PARSING_CONTEXT_HPP