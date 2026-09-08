#ifndef VNLC_LOOP_STATEMENT_NODE_HPP
#define VNLC_LOOP_STATEMENT_NODE_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include "ast/statement/VnlcControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

class VnlcLoopStatementNode : public VnlcControlFlowStatementNode {
private:
    VnlcLoopStatementNode() = delete;

    std::optional<std::unique_ptr<VnlcIdentifierNode>> label; // nullopt if no label

protected:
    VnlcLoopStatementNode(std::optional<std::unique_ptr<VnlcIdentifierNode>>&& label, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

public:
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcIdentifierNode>>& getLabel() const noexcept;
};

#endif // VNLC_LOOP_STATEMENT_NODE_HPP