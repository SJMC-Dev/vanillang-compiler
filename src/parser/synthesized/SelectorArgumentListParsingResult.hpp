#ifndef VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP
#define VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vnlc {
    struct SelectorArgumentListParsingResult {
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> arguments;
    };
} // namespace vnlc

#endif // VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP
