#ifndef VNLC_STRING_PARSING_RESULT_HPP
#define VNLC_STRING_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct StringParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_STRING_PARSING_RESULT_HPP
