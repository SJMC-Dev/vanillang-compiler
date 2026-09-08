#ifndef VNLC_LITERAL_PARSING_RESULT_HPP
#define VNLC_LITERAL_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcLiteralParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_LITERAL_PARSING_RESULT_HPP
