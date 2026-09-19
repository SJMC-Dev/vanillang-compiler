#ifndef VNLC_PRIMARY_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_PRIMARY_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct PrimaryExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_PRIMARY_EXPRESSION_PARSING_RESULT_HPP
