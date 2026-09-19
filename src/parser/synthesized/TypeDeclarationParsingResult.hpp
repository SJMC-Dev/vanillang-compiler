#ifndef VNLC_TYPE_DECLARATION_PARSING_RESULT_HPP
#define VNLC_TYPE_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/TypeDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct TypeDeclarationParsingResult {
        std::unique_ptr<TypeDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_TYPE_DECLARATION_PARSING_RESULT_HPP
