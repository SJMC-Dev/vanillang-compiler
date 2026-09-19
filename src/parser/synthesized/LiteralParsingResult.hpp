#ifndef VNLC_LITERAL_PARSING_RESULT_HPP
#define VNLC_LITERAL_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct LiteralParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_LITERAL_PARSING_RESULT_HPP
