#ifndef VNLC_PRIMITIVE_TYPE_EXPRESSION_NODE_HPP
#define VNLC_PRIMITIVE_TYPE_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"
#include "ast/expression/PrimitiveTypeExpressionKind.hpp"

namespace vnlc {
    class PrimitiveTypeExpressionNode : public PrimaryExpressionNode {
    private:
        PrimitiveTypeExpressionNode() = delete;

        PrimitiveTypeExpressionKind kind;

    public:
        PrimitiveTypeExpressionNode(PrimitiveTypeExpressionKind kind, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const PrimitiveTypeExpressionKind getKind() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_PRIMITIVE_TYPE_EXPRESSION_NODE_HPP
