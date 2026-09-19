#ifndef VNLC_CLASS_MEMBER_PARSING_RESULT_HPP
#define VNLC_CLASS_MEMBER_PARSING_RESULT_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ClassMemberParsingResult {
        std::unique_ptr<DeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_CLASS_MEMBER_PARSING_RESULT_HPP
