#ifndef VNLC_ENUM_BODY_PARSING_RESULT_HPP
#define VNLC_ENUM_BODY_PARSING_RESULT_HPP

#include "ast/declaration/VnlcEnumMemberDeclarationNode.hpp"
#include <memory>
#include <vector>

struct VnlcEnumBodyParsingResult {
    std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>> declarations;
};

#endif // VNLC_ENUM_BODY_PARSING_RESULT_HPP
