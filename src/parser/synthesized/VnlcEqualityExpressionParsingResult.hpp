#ifndef VNLC_EQUALITY_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_EQUALITY_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcEqualityExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_EQUALITY_EXPRESSION_PARSING_RESULT_HPP
