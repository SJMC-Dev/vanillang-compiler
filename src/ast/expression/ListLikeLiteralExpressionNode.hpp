#ifndef VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/ListLikeLiteralExpressionType.hpp"
#include "ast/expression/LiteralExpressionNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class ListLikeLiteralExpressionNode : public LiteralExpressionNode {
    private:
        ListLikeLiteralExpressionNode() = delete;

        ListLikeLiteralExpressionType type;
        std::vector<std::unique_ptr<ExpressionNode>> elements;

    public:
        ListLikeLiteralExpressionNode(ListLikeLiteralExpressionType type, std::vector<std::unique_ptr<ExpressionNode>>&& elements, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ListLikeLiteralExpressionType getType() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ExpressionNode>>& getElements() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_LIST_LIKE_LITERAL_EXPRESSION_NODE_HPP
