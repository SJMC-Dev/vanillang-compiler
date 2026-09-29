#ifndef VNLC_OPERATOR_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_OPERATOR_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/OperatorDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct OperatorDeclarationParsingContext {
        OperatorDeclarationKind::Context context;
        OperatorDeclarationKind::AccessModifier accessModifier;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_OPERATOR_DECLARATION_PARSING_CONTEXT_HPP
