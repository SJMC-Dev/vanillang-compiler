#include "IdentifierExpressionNode.hpp"

namespace vnlc {
    IdentifierExpressionNode::IdentifierExpressionNode(std::unique_ptr<IdentifierNode>&& name, const Token& firstToken, const Token& lastToken) noexcept
        : PrimaryExpressionNode(firstToken, lastToken),
          name(std::move(name)) {}

    const IdentifierNode& IdentifierExpressionNode::getName() const noexcept {
        return *name;
    }
} // namespace vnlc
