#ifndef VNLC_TOP_IDENTIFIER_DECLARATION_PARSING_RESULT_HPP
#define VNLC_TOP_IDENTIFIER_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include <memory>

struct VnlcTopIdentifierDeclarationParsingResult {
    std::unique_ptr<VnlcDeclarationNode> declaration;
};

#endif // VNLC_TOP_IDENTIFIER_DECLARATION_PARSING_RESULT_HPP