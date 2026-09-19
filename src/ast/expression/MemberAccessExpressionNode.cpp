#include "MemberAccessExpressionNode.hpp"

namespace vnlc {
    MemberAccessExpressionNode::MemberAccessExpressionNode(
        MemberAccessExpressionType type,
        std::unique_ptr<ExpressionNode>&& object,
        std::unique_ptr<IdentifierExpressionNode>&& member,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          type(type),
          object(std::move(object)),
          member(std::move(member)) {}

    const MemberAccessExpressionType MemberAccessExpressionNode::getType() const noexcept {
        return type;
    }

    const ExpressionNode& MemberAccessExpressionNode::getObject() const noexcept {
        return *object;
    }

    const IdentifierExpressionNode& MemberAccessExpressionNode::getMember() const noexcept {
        return *member;
    }
} // namespace vnlc
