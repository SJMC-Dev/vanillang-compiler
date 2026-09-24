#include "Token.hpp"

namespace vnlc {
    Token::Token(TokenKind kind, std::string_view value, std::size_t line, std::size_t column, std::size_t offset) : kind(kind), value(value), offset(offset), line(line), column(column) {}
    Token::Token(Token&& other) noexcept : kind(other.kind), value(std::move(other.value)), offset(other.offset), line(other.line), column(other.column) {}
    Token::Token(const Token& other) : kind(other.kind), value(other.value), offset(other.offset), line(other.line), column(other.column) {}

    Token& Token::operator=(Token&& other) noexcept {
        if (this != &other) {
            kind = other.kind;
            value = std::move(other.value);
            offset = other.offset;
            line = other.line;
            column = other.column;
        }
        return *this;
    }
    Token& Token::operator=(const Token& other) {
        if (this != &other) {
            kind = other.kind;
            value = other.value;
            offset = other.offset;
            line = other.line;
            column = other.column;
        }
        return *this;
    }

    TokenKind Token::getKind() const noexcept {
        return kind;
    }

    std::size_t Token::getLine() const noexcept {
        return line;
    }

    std::size_t Token::getColumn() const noexcept {
        return column;
    }

    std::size_t Token::getOffset() const noexcept {
        return offset;
    }

    std::size_t Token::getLength() const noexcept {
        return static_cast<std::size_t>(value.length());
    }

    std::string_view Token::getValue() const noexcept {
        return value;
    }
} // namespace vnlc
