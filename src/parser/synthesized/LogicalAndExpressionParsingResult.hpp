#ifndef VNLC_LOGICAL_AND_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_LOGICAL_AND_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct LogicalAndExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_LOGICAL_AND_EXPRESSION_PARSING_RESULT_HPP
