#ifndef VNLC_CONTINUE_STATEMENT_NODE_HPP
#define VNLC_CONTINUE_STATEMENT_NODE_HPP

#include "ast/identifier/VnlcIdentifierNode.hpp"
#include "ast/statement/VnlcControlFlowStatementNode.hpp"
#include <memory>
#include <optional>

class VnlcContinueStatementNode : public VnlcControlFlowStatementNode {
private:
    VnlcContinueStatementNode() = delete;

    std::optional<std::unique_ptr<VnlcIdentifierNode>> label; // nullopt if no label

public:
    VnlcContinueStatementNode(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;
    VnlcContinueStatementNode(std::unique_ptr<VnlcIdentifierNode>&& label, const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

    [[nodiscard]] const std::optional<std::unique_ptr<VnlcIdentifierNode>>& getLabel() const noexcept;
};

#endif // VNLC_CONTINUE_STATEMENT_NODE_HPP