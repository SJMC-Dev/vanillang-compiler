#ifndef VNLC_ASSIGNMENT_EXPRESSION_PARSING_RESULT_HPP
#define VNLC_ASSIGNMENT_EXPRESSION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcAssignmentExpressionParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_ASSIGNMENT_EXPRESSION_PARSING_RESULT_HPP
