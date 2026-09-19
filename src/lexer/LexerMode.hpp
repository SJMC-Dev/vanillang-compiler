#ifndef VNLC_LEXER_MODE_HPP
#define VNLC_LEXER_MODE_HPP

namespace vnlc {
    enum class LexerMode {
        DEFAULT,
        STRING,
        FORMAT_STRING,
        RAW_STRING,
        INTERPOLATION_BEGIN,
        NUMBER_STARTS_WITH_DOT,
    };
}

#endif // VNLC_LEXER_MODE_HPP