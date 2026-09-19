#ifndef VNLC_BITWISE_AND_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_BITWISE_AND_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct BitwiseAndExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_BITWISE_AND_EXPRESSION_PARSING_RESULT_HPP
