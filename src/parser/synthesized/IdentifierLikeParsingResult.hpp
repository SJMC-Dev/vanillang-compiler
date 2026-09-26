#ifndef VNLC_IDENTIFIER_LIKE_PARSING_RESULT_HPP
#define VNLC_IDENTIFIER_LIKE_PARSING_RESULT_HPP

#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct IdentifierLikeParsingResult {
        std::unique_ptr<IdentifierLikeExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_IDENTIFIER_LIKE_PARSING_RESULT_HPP
