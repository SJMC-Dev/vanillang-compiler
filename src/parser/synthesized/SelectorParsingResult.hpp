#ifndef VNLC_SELECTOR_PARSING_RESULT_HPP
#define VNLC_SELECTOR_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct SelectorParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_SELECTOR_PARSING_RESULT_HPP
