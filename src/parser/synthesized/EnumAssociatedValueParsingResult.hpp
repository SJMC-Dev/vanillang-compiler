#ifndef VNLC_ENUM_ASSOCIATED_VALUE_PARSING_RESULT_HPP
#define VNLC_ENUM_ASSOCIATED_VALUE_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct EnumAssociatedValueParsingResult {
        std::unique_ptr<ValueDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_ENUM_ASSOCIATED_VALUE_PARSING_RESULT_HPP
