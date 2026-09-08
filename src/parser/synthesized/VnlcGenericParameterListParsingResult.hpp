#ifndef VNLC_GENERIC_PARAMETER_LIST_PARSING_RESULT_HPP
#define VNLC_GENERIC_PARAMETER_LIST_PARSING_RESULT_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>
#include <vector>

struct VnlcGenericParameterListParsingResult {
    std::vector<std::unique_ptr<VnlcIdentifierNode>> parameters;
};

#endif // VNLC_GENERIC_PARAMETER_LIST_PARSING_RESULT_HPP
