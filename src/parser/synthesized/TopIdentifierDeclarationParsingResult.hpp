#ifndef VNLC_TOP_IDENTIFIER_DECLARATION_PARSING_RESULT_HPP
#define VNLC_TOP_IDENTIFIER_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct TopIdentifierDeclarationParsingResult {
        std::unique_ptr<DeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_TOP_IDENTIFIER_DECLARATION_PARSING_RESULT_HPP