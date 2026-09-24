#ifndef VNLC_SELECTOR_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_SELECTOR_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/LiteralExpressionNode.hpp"
#include "ast/expression/SelectorLiteralExpressionKind.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    class SelectorLiteralExpressionNode : public LiteralExpressionNode {
    private:
        SelectorLiteralExpressionNode() = delete;

        SelectorLiteralExpressionKind kind;
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> arguments;

    public:
        SelectorLiteralExpressionNode(
            SelectorLiteralExpressionKind kind,
            std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&& arguments,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const SelectorLiteralExpressionKind getKind() const noexcept;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& getArguments() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SELECTOR_LITERAL_EXPRESSION_NODE_HPP