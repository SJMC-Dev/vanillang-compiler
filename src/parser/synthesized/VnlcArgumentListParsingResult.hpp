#ifndef VNLC_ARGUMENT_LIST_PARSING_RESULT_HPP
#define VNLC_ARGUMENT_LIST_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include <memory>
#include <optional>
#include <vector>

struct VnlcArgumentListParsingResult {
    std::vector<std::unique_ptr<VnlcExpressionNode>> arguments;
    std::optional<std::unique_ptr<VnlcExpressionNode>> context;
};

#endif // VNLC_ARGUMENT_LIST_PARSING_RESULT_HPP
