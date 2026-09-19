#ifndef VNLC_SWITCH_CASE_PARSING_RESULT_HPP
#define VNLC_SWITCH_CASE_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/SwitchStatementType.hpp"
#include "ast/type/TypeNode.hpp"
#include <optional>

namespace vnlc {
    struct SwitchCaseParsingResult {
        SwitchStatementType kind;
        std::optional<std::unique_ptr<ExpressionNode>> literal;
        std::optional<std::unique_ptr<TypeNode>> type;
        std::optional<std::unique_ptr<ExpressionNode>> guardExpression;
    };
} // namespace vnlc

#endif // VNLC_SWITCH_CASE_PARSING_RESULT_HPP
