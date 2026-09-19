#ifndef VNLC_SIMPLE_LITERAL_EXPRESSION_TYPE_HPP
#define VNLC_SIMPLE_LITERAL_EXPRESSION_TYPE_HPP

namespace vnlc {
    enum class SimpleLiteralExpressionType {
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

#endif // VNLC_SIMPLE_LITERAL_EXPRESSION_TYPE_HPP