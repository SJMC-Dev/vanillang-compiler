#include "BreakStatementNode.hpp"
#include <memory>

namespace vnlc {
    BreakStatementNode::BreakStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), label(std::nullopt) {}

    BreakStatementNode::BreakStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::make_optional(std::move(label))) {}

    const std::optional<std::unique_ptr<IdentifierNode>>& BreakStatementNode::getLabel() const noexcept {
        return label;
    }
} // namespace vnlc
