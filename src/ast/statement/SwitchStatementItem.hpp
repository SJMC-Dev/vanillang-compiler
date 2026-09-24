#ifndef VNLC_SWITCH_STATEMENT_ITEM_HPP
#define VNLC_SWITCH_STATEMENT_ITEM_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    namespace SwitchStatementItem {
        struct LiteralMatchItem {
            std::unique_ptr<ExpressionNode> literal;
            std::unique_ptr<StatementNode> body;
        };

        struct TypeMatchItem {
            std::unique_ptr<TypeReferenceNode> type;
            std::unique_ptr<StatementNode> body;
            std::optional<std::unique_ptr<ExpressionNode>> guardExpression;
        };
    } // namespace SwitchStatementItem
} // namespace vnlc

#endif // VNLC_SWITCH_STATEMENT_ITEM_HPP