#ifndef VNLC_ENUM_MEMBER_DECLARATION_PARSING_RESULT_HPP
#define VNLC_ENUM_MEMBER_DECLARATION_PARSING_RESULT_HPP

#include "ast/declaration/VnlcEnumMemberDeclarationNode.hpp"
#include <memory>

struct VnlcEnumMemberDeclarationParsingResult {
    std::unique_ptr<VnlcEnumMemberDeclarationNode> declaration;
};

#endif // VNLC_ENUM_MEMBER_DECLARATION_PARSING_RESULT_HPP
