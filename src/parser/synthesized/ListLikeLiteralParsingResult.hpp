#ifndef VNLC_LIST_LIKE_LITERAL_PARSING_RESULT_HPP
#define VNLC_LIST_LIKE_LITERAL_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct ListLikeLiteralParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_LIST_LIKE_LITERAL_PARSING_RESULT_HPP
