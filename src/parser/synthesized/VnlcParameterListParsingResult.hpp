#ifndef VNLC_PARAMETER_LIST_PARSING_RESULT_HPP
#define VNLC_PARAMETER_LIST_PARSING_RESULT_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include <memory>
#include <vector>

struct VnlcParameterListParsingResult {
    std::vector<std::unique_ptr<VnlcValueDeclarationNode>> parameters;
};

#endif // VNLC_PARAMETER_LIST_PARSING_RESULT_HPP
