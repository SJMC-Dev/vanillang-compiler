#ifndef VNLC_ENUM_ASSOCIATED_VALUE_LIST_PARSING_RESULT_HPP
#define VNLC_ENUM_ASSOCIATED_VALUE_LIST_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct EnumAssociatedValueListParsingResult {
        std::vector<std::unique_ptr<ValueDeclarationNode>> associatedValues;
    };
} // namespace vnlc

#endif // VNLC_ENUM_ASSOCIATED_VALUE_LIST_PARSING_RESULT_HPP
