#ifndef VNLC_EQUALITY_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_EQUALITY_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct EqualityExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_EQUALITY_EXPRESSION_PARSING_RESULT_HPP
