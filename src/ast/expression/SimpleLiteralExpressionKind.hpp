#ifndef VNLC_SIMPLE_LITERAL_EXPRESSION_KIND_HPP
#define VNLC_SIMPLE_LITERAL_EXPRESSION_KIND_HPP

namespace vnlc {
    enum class SimpleLiteralExpressionKind {
        DECIMAL_BYTE,
        DECIMAL_SHORT,
        DECIMAL_INTEGER,
        DECIMAL_LONG,
        DECIMAL_FLOAT,
        DECIMAL_DOUBLE,

        BINARY,
        OCTAL,
        HEXADECIMAL,

        CHARACTER,

        BOOLEAN,
    };
}

#endif // VNLC_SIMPLE_LITERAL_EXPRESSION_KIND_HPP