#ifndef VNLC_GENERIC_ARGUMENT_LIST_PARSING_RESULT_HPP
#define VNLC_GENERIC_ARGUMENT_LIST_PARSING_RESULT_HPP

#include "ast/type/VnlcTypeNode.hpp"
#include <memory>
#include <vector>

struct VnlcGenericArgumentListParsingResult {
    std::vector<std::unique_ptr<VnlcTypeNode>> arguments;
};

#endif // VNLC_GENERIC_ARGUMENT_LIST_PARSING_RESULT_HPP
