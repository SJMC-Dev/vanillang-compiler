#ifndef VNLC_SWITCH_STATEMENT_ITEM_HPP
#define VNLC_SWITCH_STATEMENT_ITEM_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/statement/VnlcStatementNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <memory>
#include <optional>

namespace VnlcSwitchStatementItem {
    struct LiteralMatchItem {
        std::unique_ptr<VnlcExpressionNode> literal;
        std::unique_ptr<VnlcStatementNode> body;
    };

    struct TypeMatchItem {
        std::unique_ptr<VnlcTypeNode> type;
        std::unique_ptr<VnlcStatementNode> body;
        std::optional<std::unique_ptr<VnlcExpressionNode>> guardExpression;
    };
} // namespace VnlcSwitchStatementItem

#endif // VNLC_SWITCH_STATEMENT_ITEM_HPP