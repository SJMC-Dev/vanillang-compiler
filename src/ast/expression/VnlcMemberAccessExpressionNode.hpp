#ifndef VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP
#define VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP

#include "ast/expression/VnlcExpressionNode.hpp"
#include "ast/expression/VnlcIdentifierExpressionNode.hpp"
#include "ast/expression/VnlcMemberAccessExpressionType.hpp"
#include <memory>

class VnlcMemberAccessExpressionNode : public VnlcExpressionNode {
private:
    VnlcMemberAccessExpressionNode() = delete;

    VnlcMemberAccessExpressionType type;
    std::unique_ptr<VnlcExpressionNode> object;
    std::unique_ptr<VnlcIdentifierExpressionNode> member;

public:
    VnlcMemberAccessExpressionNode(
        VnlcMemberAccessExpressionType type,
        std::unique_ptr<VnlcExpressionNode>&& object,
        std::unique_ptr<VnlcIdentifierExpressionNode>&& member,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const VnlcMemberAccessExpressionType getType() const noexcept;
    [[nodiscard]] const VnlcExpressionNode& getObject() const noexcept;
    [[nodiscard]] const VnlcIdentifierExpressionNode& getMember() const noexcept;
};

#endif // VNLC_MEMBER_ACCESS_EXPRESSION_NODE_HPP
