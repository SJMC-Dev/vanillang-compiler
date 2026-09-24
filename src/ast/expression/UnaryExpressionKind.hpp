#ifndef VNLC_UNARY_EXPRESSION_KIND_HPP
#define VNLC_UNARY_EXPRESSION_KIND_HPP

namespace vnlc {
    enum class UnaryExpressionKind {
        UNARY_PLUS,
        UNARY_MINUS,
        LOGICAL_NOT,
        BITWISE_NOT,
    };
}

#endif // VNLC_UNARY_EXPRESSION_KIND_HPP