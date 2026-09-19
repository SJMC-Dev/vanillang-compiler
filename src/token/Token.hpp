#ifndef VNLC_TOKEN_HPP
#define VNLC_TOKEN_HPP

#include "token/TokenType.hpp"

#include <string>
#include <string_view>

namespace vnlc {
    class Token {
    private:
        TokenType type;
        std::string value;
        unsigned int offset;
        unsigned int line;
        unsigned int column;

    public:
        Token(TokenType type, std::string_view value, unsigned int line, unsigned int column, unsigned int offset);
        Token(Token&& other) noexcept;
        Token(const Token& other);

        Token& operator=(Token&& other) noexcept;
        Token& operator=(const Token& other);

        [[nodiscard]] TokenType getType() const noexcept;
        [[nodiscard]] unsigned int getLine() const noexcept;
        [[nodiscard]] unsigned int getColumn() const noexcept;
        [[nodiscard]] unsigned int getOffset() const noexcept;
        [[nodiscard]] unsigned int getLength() const noexcept;
        [[nodiscard]] std::string_view getValue() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TOKEN_HPP
