#ifndef VNLC_SELECTOR_ARGUMENT_PARSING_RESULT_HPP
#define VNLC_SELECTOR_ARGUMENT_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>
#include <string>

struct VnlcSelectorArgumentParsingResult {
    std::string key;
    std::unique_ptr<VnlcExpressionNode> value;
};

#endif // VNLC_SELECTOR_ARGUMENT_PARSING_RESULT_HPP
