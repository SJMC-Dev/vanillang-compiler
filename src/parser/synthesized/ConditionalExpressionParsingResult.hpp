#ifndef VNLC_CONDITIONAL_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_CONDITIONAL_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct ConditionalExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_CONDITIONAL_EXPRESSION_PARSING_RESULT_HPP
