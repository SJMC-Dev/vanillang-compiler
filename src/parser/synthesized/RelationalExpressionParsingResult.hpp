#ifndef VNLC_RELATIONAL_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_RELATIONAL_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct RelationalExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_RELATIONAL_EXPRESSION_PARSING_RESULT_HPP
