#ifndef VNLC_TYPE_ALIAS_DECLARATION_PARSING_RESULT_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct TypeAliasDeclarationParsingResult {
        std::unique_ptr<TypeAliasDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_TYPE_ALIAS_DECLARATION_PARSING_RESULT_HPP
