#ifndef VNLC_BITWISE_OR_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_BITWISE_OR_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct BitwiseOrExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_BITWISE_OR_EXPRESSION_PARSING_RESULT_HPP
