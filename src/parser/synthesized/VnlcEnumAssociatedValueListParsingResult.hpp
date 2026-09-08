#ifndef VNLC_ENUM_ASSOCIATED_VALUE_LIST_PARSING_RESULT_HPP
#define VNLC_ENUM_ASSOCIATED_VALUE_LIST_PARSING_RESULT_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include <memory>
#include <vector>

struct VnlcEnumAssociatedValueListParsingResult {
    std::vector<std::unique_ptr<VnlcValueDeclarationNode>> associatedValues;
};

#endif // VNLC_ENUM_ASSOCIATED_VALUE_LIST_PARSING_RESULT_HPP
