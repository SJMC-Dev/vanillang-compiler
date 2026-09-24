#ifndef VNLC_STRING_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_STRING_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/LiteralExpressionNode.hpp"
#include "ast/expression/StringLiteralExpressionKind.hpp"
#include <memory>
#include <variant>
#include <vector>

namespace vnlc {
    class StringLiteralExpressionNode : public LiteralExpressionNode {
    private:
        StringLiteralExpressionNode() = delete;

        StringLiteralExpressionKind kind;
        std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>> parts;

    public:
        StringLiteralExpressionNode(
            StringLiteralExpressionKind kind,
            std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>&& parts,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const StringLiteralExpressionKind getKind() const noexcept;
        [[nodiscard]] const std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>& getParts() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_STRING_LITERAL_EXPRESSION_NODE_HPP