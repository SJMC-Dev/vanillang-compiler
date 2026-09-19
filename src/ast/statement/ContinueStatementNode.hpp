#ifndef VNLC_CONTINUE_STATEMENT_NODE_HPP
#define VNLC_CONTINUE_STATEMENT_NODE_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    class ContinueStatementNode : public ControlFlowStatementNode {
    private:
        ContinueStatementNode() = delete;

        std::optional<std::unique_ptr<IdentifierNode>> label; // nullopt if no label

    public:
        ContinueStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        ContinueStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const std::optional<std::unique_ptr<IdentifierNode>>& getLabel() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CONTINUE_STATEMENT_NODE_HPP