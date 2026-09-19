#ifndef VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP
#define VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/IdentifierExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionType.hpp"
#include <memory>

namespace vnlc {
    class MemberAccessExpressionNode : public ExpressionNode {
    private:
        MemberAccessExpressionNode() = delete;

        MemberAccessExpressionType type;
        std::unique_ptr<ExpressionNode> object;
        std::unique_ptr<IdentifierExpressionNode> member;

    public:
        MemberAccessExpressionNode(
            MemberAccessExpressionType type,
            std::unique_ptr<ExpressionNode>&& object,
            std::unique_ptr<IdentifierExpressionNode>&& member,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const MemberAccessExpressionType getType() const noexcept;
        [[nodiscard]] const ExpressionNode& getObject() const noexcept;
        [[nodiscard]] const IdentifierExpressionNode& getMember() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP
