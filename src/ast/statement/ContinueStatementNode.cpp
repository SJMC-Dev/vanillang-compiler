#include "ContinueStatementNode.hpp"

namespace vnlc {
    ContinueStatementNode::ContinueStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), label(std::nullopt) {}

    ContinueStatementNode::ContinueStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::make_optional(std::move(label))) {}

    const std::optional<std::unique_ptr<IdentifierNode>>& ContinueStatementNode::getLabel() const noexcept {
        return label;
    }
} // namespace vnlc
