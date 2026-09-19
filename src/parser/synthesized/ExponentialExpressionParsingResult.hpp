#ifndef VNLC_EXPONENTIAL_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_EXPONENTIAL_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct ExponentialExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_EXPONENTIAL_EXPRESSION_PARSING_RESULT_HPP
