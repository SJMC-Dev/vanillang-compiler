#ifndef VNLC_SELECTOR_ARGUMENT_PARSING_RESULT_HPP
#define VNLC_SELECTOR_ARGUMENT_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <string>

namespace vnlc {
    struct SelectorArgumentParsingResult {
        std::string key;
        std::unique_ptr<ExpressionNode> value;
    };
} // namespace vnlc

#endif // VNLC_SELECTOR_ARGUMENT_PARSING_RESULT_HPP
