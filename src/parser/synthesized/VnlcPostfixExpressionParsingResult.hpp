#ifndef VNLC_POSTFIX_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_POSTFIX_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcPostfixExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_POSTFIX_EXPRESSION_PARSING_RESULT_HPP
