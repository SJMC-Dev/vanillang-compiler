#ifndef VNLC_LOOP_STATEMENT_NODE_HPP
#define VNLC_LOOP_STATEMENT_NODE_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    class LoopStatementNode : public ControlFlowStatementNode {
    private:
        LoopStatementNode() = delete;

        std::optional<std::unique_ptr<IdentifierNode>> label; // nullopt if no label

    protected:
        LoopStatementNode(std::optional<std::unique_ptr<IdentifierNode>>&& label, const Token& firstToken, const Token& lastToken) noexcept;

    public:
        [[nodiscard]] const std::optional<std::unique_ptr<IdentifierNode>>& getLabel() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_LOOP_STATEMENT_NODE_HPP