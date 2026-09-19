#ifndef VNLC_NULLISH_COALESCING_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_NULLISH_COALESCING_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct NullishCoalescingExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_NULLISH_COALESCING_EXPRESSION_PARSING_RESULT_HPP
