#ifndef VNLC_SHIFT_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_SHIFT_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct ShiftExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_SHIFT_EXPRESSION_PARSING_RESULT_HPP
