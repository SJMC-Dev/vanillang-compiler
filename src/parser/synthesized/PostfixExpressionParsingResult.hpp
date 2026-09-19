#ifndef VNLC_POSTFIX_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_POSTFIX_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct PostfixExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_POSTFIX_EXPRESSION_PARSING_RESULT_HPP
