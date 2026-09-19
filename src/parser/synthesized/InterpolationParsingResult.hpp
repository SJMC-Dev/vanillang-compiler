#ifndef VNLC_INTERPOLATION_PARSING_RESULT_HPP
#define VNLC_INTERPOLATION_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct InterpolationParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc

#endif // VNLC_INTERPOLATION_PARSING_RESULT_HPP
