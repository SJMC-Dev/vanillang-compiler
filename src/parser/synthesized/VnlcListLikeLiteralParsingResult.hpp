#ifndef VNLC_LIST_LIKE_LITERAL_PARSING_RESULT_HPP
#define VNLC_LIST_LIKE_LITERAL_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcListLikeLiteralParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_LIST_LIKE_LITERAL_PARSING_RESULT_HPP
