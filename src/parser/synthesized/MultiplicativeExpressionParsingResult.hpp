#ifndef VNLC_MULTIPLICATIVE_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_MULTIPLICATIVE_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct MultiplicativeExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_MULTIPLICATIVE_EXPRESSION_PARSING_RESULT_HPP
