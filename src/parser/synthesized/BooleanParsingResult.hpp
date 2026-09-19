#ifndef VNLC_BOOLEAN_PARSING_RESULT_HPP
#define VNLC_BOOLEAN_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct BooleanParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_BOOLEAN_PARSING_RESULT_HPP
