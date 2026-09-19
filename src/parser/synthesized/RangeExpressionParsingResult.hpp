#ifndef VNLC_RANGE_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_RANGE_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct RangeExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_RANGE_EXPRESSION_PARSING_RESULT_HPP
