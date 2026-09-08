#ifndef VNLC_STRING_PARSING_RESULT_HPP
#define VNLC_STRING_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcStringParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_STRING_PARSING_RESULT_HPP
