#ifndef VNLC_PARAMETER_PARSING_RESULT_HPP
#define VNLC_PARAMETER_PARSING_RESULT_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include <memory>

struct VnlcParameterParsingResult {
    std::unique_ptr<VnlcValueDeclarationNode> declaration;
};

#endif // VNLC_PARAMETER_PARSING_RESULT_HPP
