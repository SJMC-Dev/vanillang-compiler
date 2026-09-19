#ifndef VNLC_ENUM_MEMBER_DECLARATION_PARSING_RESULT_HPP
#define VNLC_ENUM_MEMBER_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct EnumMemberDeclarationParsingResult {
        std::unique_ptr<EnumMemberDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_ENUM_MEMBER_DECLARATION_PARSING_RESULT_HPP
