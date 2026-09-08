#ifndef VNLC_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_EXPRESSION_PARSING_RESULT_HPP
