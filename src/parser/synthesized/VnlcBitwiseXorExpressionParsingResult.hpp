#ifndef VNLC_BITWISE_XOR_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_BITWISE_XOR_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcBitwiseXorExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_BITWISE_XOR_EXPRESSION_PARSING_RESULT_HPP
