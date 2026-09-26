#include "MemberAccessExpressionNode.hpp"

namespace vnlc {
    MemberAccessExpressionNode::MemberAccessExpressionNode(
        MemberAccessExpressionKind kind,
        std::unique_ptr<ExpressionNode>&& object,
        std::unique_ptr<IdentifierLikeExpressionNode>&& member,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          kind(kind),
          object(std::move(object)),
          member(std::move(member)) {}

    const MemberAccessExpressionKind MemberAccessExpressionNode::getKind() const noexcept {
        return kind;
    }

    const ExpressionNode& MemberAccessExpressionNode::getObject() const noexcept {
        return *object;
    }

    const IdentifierLikeExpressionNode& MemberAccessExpressionNode::getMember() const noexcept {
        return *member;
    }
} // namespace vnlc
