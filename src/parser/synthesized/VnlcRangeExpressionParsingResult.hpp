#ifndef VNLC_RANGE_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_RANGE_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcRangeExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_RANGE_EXPRESSION_PARSING_RESULT_HPP
