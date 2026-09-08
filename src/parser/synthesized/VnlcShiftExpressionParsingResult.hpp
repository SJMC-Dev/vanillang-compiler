#ifndef VNLC_SHIFT_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_SHIFT_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcShiftExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_SHIFT_EXPRESSION_PARSING_RESULT_HPP
