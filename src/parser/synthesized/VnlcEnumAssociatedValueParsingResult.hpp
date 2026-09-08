#ifndef VNLC_ENUM_ASSOCIATED_VALUE_PARSING_RESULT_HPP
#define VNLC_ENUM_ASSOCIATED_VALUE_PARSING_RESULT_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include <memory>

struct VnlcEnumAssociatedValueParsingResult {
    std::unique_ptr<VnlcValueDeclarationNode> declaration;
};

#endif // VNLC_ENUM_ASSOCIATED_VALUE_PARSING_RESULT_HPP
