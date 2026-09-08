#ifndef VNLC_CLASS_MEMBER_PARSING_RESULT_HPP
#define VNLC_CLASS_MEMBER_PARSING_RESULT_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include <memory>

struct VnlcClassMemberParsingResult {
    std::unique_ptr<VnlcDeclarationNode> declaration;
};

#endif // VNLC_CLASS_MEMBER_PARSING_RESULT_HPP
