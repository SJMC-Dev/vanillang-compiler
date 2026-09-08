#ifndef VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP
#define VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>
#include <string>
#include <unordered_map>

struct VnlcSelectorArgumentListParsingResult {
    std::unordered_map<std::string, std::unique_ptr<VnlcExpressionNode>> arguments;
};

#endif // VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP
