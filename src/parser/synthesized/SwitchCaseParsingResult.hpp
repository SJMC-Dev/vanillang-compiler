#ifndef VNLC_SWITCH_CASE_PARSING_RESULT_HPP
#define VNLC_SWITCH_CASE_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/SwitchStatementKind.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <optional>

namespace vnlc {
    struct SwitchCaseParsingResult {
        SwitchStatementKind kind;
        std::optional<std::unique_ptr<ExpressionNode>> literal;
        std::optional<std::unique_ptr<TypeReferenceNode>> type;
        std::optional<std::unique_ptr<ExpressionNode>> guardExpression;
    };
} // namespace vnlc

#endif // VNLC_SWITCH_CASE_PARSING_RESULT_HPP
