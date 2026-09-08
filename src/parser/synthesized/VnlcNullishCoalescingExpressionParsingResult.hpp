#ifndef VNLC_NULLISH_COALESCING_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_NULLISH_COALESCING_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcNullishCoalescingExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_NULLISH_COALESCING_EXPRESSION_PARSING_RESULT_HPP
