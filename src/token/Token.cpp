#include "Token.hpp"

namespace vnlc {
    Token::Token(TokenType type, std::string_view value, unsigned int line, unsigned int column, unsigned int offset) : type(type), value(value), offset(offset), line(line), column(column) {}
    Token::Token(Token&& other) noexcept : type(other.type), value(std::move(other.value)), offset(other.offset), line(other.line), column(other.column) {}
    Token::Token(const Token& other) : type(other.type), value(other.value), offset(other.offset), line(other.line), column(other.column) {}

    Token& Token::operator=(Token&& other) noexcept {
        if (this != &other) {
            type = other.type;
            value = std::move(other.value);
            offset = other.offset;
            line = other.line;
            column = other.column;
        }
        return *this;
    }
    Token& Token::operator=(const Token& other) {
        if (this != &other) {
            type = other.type;
            value = other.value;
            offset = other.offset;
            line = other.line;
            column = other.column;
        }
        return *this;
    }

    TokenType Token::getType() const noexcept {
        return type;
    }

    unsigned int Token::getLine() const noexcept {
        return line;
    }

    unsigned int Token::getColumn() const noexcept {
        return column;
    }

    unsigned int Token::getOffset() const noexcept {
        return offset;
    }

    unsigned int Token::getLength() const noexcept {
        return static_cast<unsigned int>(value.length());
    }

    std::string_view Token::getValue() const noexcept {
        return value;
    }
} // namespace vnlc
