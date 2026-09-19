#ifndef VNLC_ENUM_BODY_PARSING_RESULT_HPP
#define VNLC_ENUM_BODY_PARSING_RESULT_HPP

#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct EnumBodyParsingResult {
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> declarations;
    };
} // namespace vnlc

#endif // VNLC_ENUM_BODY_PARSING_RESULT_HPP
