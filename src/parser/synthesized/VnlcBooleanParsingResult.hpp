#ifndef VNLC_BOOLEAN_PARSING_RESULT_HPP
#define VNLC_BOOLEAN_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcBooleanParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_BOOLEAN_PARSING_RESULT_HPP
