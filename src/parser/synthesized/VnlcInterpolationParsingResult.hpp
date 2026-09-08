#ifndef VNLC_INTERPOLATION_PARSING_RESULT_HPP
#define VNLC_INTERPOLATION_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcInterpolationParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_INTERPOLATION_PARSING_RESULT_HPP
