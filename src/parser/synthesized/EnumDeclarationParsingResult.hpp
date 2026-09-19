#ifndef VNLC_ENUM_DECLARATION_PARSING_RESULT_HPP
#define VNLC_ENUM_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/EnumDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct EnumDeclarationParsingResult {
        std::unique_ptr<EnumDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_ENUM_DECLARATION_PARSING_RESULT_HPP
