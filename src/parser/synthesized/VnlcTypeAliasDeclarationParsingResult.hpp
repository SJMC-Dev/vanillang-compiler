#ifndef VNLC_TYPE_ALIAS_DECLARATION_PARSING_RESULT_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcTypeAliasDeclarationNode.hpp"
#include <memory>

struct VnlcTypeAliasDeclarationParsingResult {
    std::unique_ptr<VnlcTypeAliasDeclarationNode> declaration;
};

#endif // VNLC_TYPE_ALIAS_DECLARATION_PARSING_RESULT_HPP
