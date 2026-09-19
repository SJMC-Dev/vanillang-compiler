#include "LoopStatementNode.hpp"

namespace vnlc {
    LoopStatementNode::LoopStatementNode(std::optional<std::unique_ptr<IdentifierNode>>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::move(label)) {}

    const std::optional<std::unique_ptr<IdentifierNode>>& LoopStatementNode::getLabel() const noexcept {
        return label;
    }
} // namespace vnlc
