#ifndef VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP
#define VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionKind.hpp"
#include <memory>

namespace vnlc {
    class MemberAccessExpressionNode : public ExpressionNode {
    private:
        MemberAccessExpressionNode() = delete;

        MemberAccessExpressionKind kind;
        std::unique_ptr<ExpressionNode> object;
        std::unique_ptr<IdentifierLikeExpressionNode> member;

    public:
        MemberAccessExpressionNode(
            MemberAccessExpressionKind kind,
            std::unique_ptr<ExpressionNode>&& object,
            std::unique_ptr<IdentifierLikeExpressionNode>&& member,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const MemberAccessExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getObject() const noexcept;
        [[nodiscard]] const IdentifierLikeExpressionNode& getMember() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP
