#ifndef VNLC_ASSIGNMENT_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_ASSIGNMENT_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct AssignmentExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_ASSIGNMENT_EXPRESSION_PARSING_RESULT_HPP
