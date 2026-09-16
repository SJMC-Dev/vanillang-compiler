#ifndef VNLC_TOKEN_TYPE_HPP
#define VNLC_TOKEN_TYPE_HPP

enum class VnlcTokenType {
    IDENTIFIER,

    NUMBER,
    CHAR,
    STRING,

    VAR,
    LET,
    CONST,

    BYTE_TYPE,
    SHORT_TYPE,
    INT_TYPE,
    LONG_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    BOOL_TYPE,
    STRING_TYPE,

    FUNC,
    INIT,
    RETURN,
    IF,
    ELSE,
    FOR,
    WHILE,
    LABEL,
    BREAK,
    CONTINUE,
    SWITCH,
    CASE,
    DEFAULT,
    WHEN,
    CONTEXT,
    VOID,
    NATIVE,
    CALLEE,
    IN,
    RELOAD,

    CLASS,
    INTERFACE,
    ABSTRACT,
    TYPE,
    ENUM,
    RECORD,
    EXTENDS,
    IMPLEMENTS,
    THIS,
    SUPER,
    PRIVATE,
    PROTECTED,
    PUBLIC,
    READONLY,
    STATIC,
    INSTANCEOF,
    FINAL,
    OVERRIDE,
    NONE,

    TRUE,
    FALSE,

    IMPORT,
    EXPORT,
    AS,
    MODULE,
    PARENT,
    SELF,

    LAMBDA,
    COMMAND,
    ASYNC,
    AWAIT,
    GENERATOR,
    YIELD,
    FROM,
    DECORATOR,
    DECORATE,
    PANIC,
    ASSERT,
    TYPEOF,
    DEFER,

    ASSET,
    DEFINE,

    METADATA,

    PLUS,                  // +
    MINUS,                 // -
    ASTERISK,              // *
    SLASH,                 // /
    DOUBLE_SLASH,          // //
    PERCENT,               // %
    DOUBLE_ASTERISK,       // **
    PLUS_EQUAL,            // +=
    MINUS_EQUAL,           // -=
    ASTERISK_EQUAL,        // *=
    SLASH_EQUAL,           // /=
    DOUBLE_SLASH_EQUAL,    // //=
    PERCENT_EQUAL,         // %=
    DOUBLE_ASTERISK_EQUAL, // **=

    EQUAL,                 // =
    DOUBLE_QUESTION_EQUAL, // ??=

    DOUBLE_EQUAL,      // ==
    EXCLAMATION_EQUAL, // !=
    RIGHT_ANGLE,       // >
    RIGHT_ANGLE_EQUAL, // >=
    LEFT_ANGLE,        // <
    LEFT_ANGLE_EQUAL,  // <=

    DOUBLE_AMPERSAND, // &&
    DOUBLE_PIPE,      // ||
    DOUBLE_QUESTION,  // ??
    EXCLAMATION,      // !

    ARROW, // ->

    AMPERSAND,                // &
    PIPE,                     // |
    CARET,                    // ^
    TILDE,                    // ~
    DOUBLE_LEFT_ANGLE,        // <<
    DOUBLE_RIGHT_ANGLE,       // >>
    TRIPLE_RIGHT_ANGLE,       // >>>
    AMPERSAND_EQUAL,          // &=
    PIPE_EQUAL,               // |=
    CARET_EQUAL,              // ^=
    DOUBLE_LEFT_ANGLE_EQUAL,  // <<=
    DOUBLE_RIGHT_ANGLE_EQUAL, // >>=
    TRIPLE_RIGHT_ANGLE_EQUAL, // >>>=

    DOT,             // .
    COMMA,           // ,
    SEMICOLON,       // ;
    COLON,           // :
    DOUBLE_COLON,    // ::
    QUESTION,        // ?
    QUESTION_DOT,    // ?.
    EXCLAMATION_DOT, // !.
    DOUBLE_DOT,      // ..
    TRIPLE_DOT,      // ...

    LEFT_PARENTHESIS,  // (
    RIGHT_PARENTHESIS, // )
    LEFT_BRACE,        // {
    RIGHT_BRACE,       // }
    LEFT_BRACKET,      // [
    RIGHT_BRACKET,     // ]

    SELECTOR_PREFIX, // @p, @r, @a, @e, @s, @n

    INTERPOLATION_START, // $(

    SINGLE_LINE_COMMENT, // # comment
    MULTI_LINE_COMMENT,  // #* comment *#

    BLANK,
    NEWLINE,

    LEXICAL_ERROR,

    END_OF_FILE,
};

#endif // VNLC_TOKEN_TYPE_HPP