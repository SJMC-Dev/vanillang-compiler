#ifndef VNLC_SELECTOR_PARSING_RESULT_HPP
#define VNLC_SELECTOR_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>

struct VnlcSelectorParsingResult {
    std::unique_ptr<VnlcExpressionNode> expression;
};

#endif // VNLC_SELECTOR_PARSING_RESULT_HPP
