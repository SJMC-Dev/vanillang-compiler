#ifndef VNLC_STRING_LITERAL_EXPRESSION_KIND_HPP
#define VNLC_STRING_LITERAL_EXPRESSION_KIND_HPP

namespace vnlc {
    enum class StringLiteralExpressionKind {
        STRING,
        FORMAT_STRING,
        RAW_STRING,
    };
}

#endif // VNLC_STRING_LITERAL_EXPRESSION_KIND_HPP