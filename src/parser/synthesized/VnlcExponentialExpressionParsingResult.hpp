#ifndef VNLC_EXPONENTIAL_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_EXPONENTIAL_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcExponentialExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_EXPONENTIAL_EXPRESSION_PARSING_RESULT_HPP
