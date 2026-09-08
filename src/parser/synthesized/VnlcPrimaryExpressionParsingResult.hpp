#ifndef VNLC_PRIMARY_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_PRIMARY_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcPrimaryExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_PRIMARY_EXPRESSION_PARSING_RESULT_HPP
