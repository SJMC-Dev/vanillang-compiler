#ifndef VNLC_VARIABLE_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_VARIABLE_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/VnlcDeclarationItem.hpp"
#include <vector>

struct VnlcVariableDeclarationParsingContext {
    enum class Position {
        TOP_LEVEL,
        STATEMENT,
    };

    Position position;
    bool hasMetadata;
    std::vector<VnlcDeclarationItem::MetadataTerm> metadataTerms;
};

#endif // VNLC_VARIABLE_DECLARATION_PARSING_CONTEXT_HPP