#ifndef VNLC_ADDITIVE_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_ADDITIVE_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct AdditiveExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_ADDITIVE_EXPRESSION_PARSING_RESULT_HPP
