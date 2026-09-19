#ifndef VNLC_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct ExpressionParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_EXPRESSION_PARSING_RESULT_HPP
