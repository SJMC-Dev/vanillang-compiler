#ifndef VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/ListLikeLiteralExpressionKind.hpp"
#include "ast/expression/LiteralExpressionNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class ListLikeLiteralExpressionNode : public LiteralExpressionNode {
    private:
        ListLikeLiteralExpressionNode() = delete;

        ListLikeLiteralExpressionKind kind;
        std::vector<std::unique_ptr<ExpressionNode>> elements;

    public:
        ListLikeLiteralExpressionNode(ListLikeLiteralExpressionKind kind, std::vector<std::unique_ptr<ExpressionNode>>&& elements, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ListLikeLiteralExpressionKind getKind() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ExpressionNode>>& getElements() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP
