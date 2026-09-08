#ifndef VNLC_BREAK_STATEMENT_NODE_HPP
#define VNLC_BREAK_STATEMENT_NODE_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include "ast/statement/VnlcControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

class VnlcBreakStatementNode : public VnlcControlFlowStatementNode {
private:
    VnlcBreakStatementNode() = delete;

    std::optional<std::unique_ptr<VnlcIdentifierNode>> label; // nullopt if no label

public:
    VnlcBreakStatementNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
    VnlcBreakStatementNode(std::unique_ptr<VnlcIdentifierNode>&& label, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const std::optional<std::unique_ptr<VnlcIdentifierNode>>& getLabel() const noexcept;
};

#endif // VNLC_BREAK_STATEMENT_NODE_HPP