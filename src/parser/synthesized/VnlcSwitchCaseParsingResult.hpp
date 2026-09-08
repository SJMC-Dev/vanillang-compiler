#ifndef VNLC_SWITCH_CASE_PARSING_RESULT_HPP
#define VNLC_SWITCH_CASE_PARSING_RESULT_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/statement/VnlcSwitchStatementType.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <optional>

struct VnlcSwitchCaseParsingResult {
    VnlcSwitchStatementType kind;
    std::optional<std::unique_ptr<VnlcExpressionNode>> literal;
    std::optional<std::unique_ptr<VnlcTypeNode>> type;
    std::optional<std::unique_ptr<VnlcExpressionNode>> guardExpression;
};

#endif // VNLC_SWITCH_CASE_PARSING_RESULT_HPP
