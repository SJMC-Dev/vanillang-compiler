#ifndef VNLC_RELATIONAL_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_RELATIONAL_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcRelationalExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_RELATIONAL_EXPRESSION_PARSING_RESULT_HPP
