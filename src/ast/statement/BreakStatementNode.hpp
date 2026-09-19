#ifndef VNLC_BREAK_STATEMENT_NODE_HPP
#define VNLC_BREAK_STATEMENT_NODE_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    class BreakStatementNode : public ControlFlowStatementNode {
    private:
        BreakStatementNode() = delete;

        std::optional<std::unique_ptr<IdentifierNode>> label; // nullopt if no label

    public:
        BreakStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        BreakStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const std::optional<std::unique_ptr<IdentifierNode>>& getLabel() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_BREAK_STATEMENT_NODE_HPP