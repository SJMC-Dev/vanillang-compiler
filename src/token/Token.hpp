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
        std::size_t offset;
        std::size_t line;
        std::size_t column;

    public:
        Token(TokenType type, std::string_view value, std::size_t line, std::size_t column, std::size_t offset);
        Token(Token&& other) noexcept;
        Token(const Token& other);

        Token& operator=(Token&& other) noexcept;
        Token& operator=(const Token& other);

        [[nodiscard]] TokenType getType() const noexcept;
        [[nodiscard]] std::size_t getLine() const noexcept;
        [[nodiscard]] std::size_t getColumn() const noexcept;
        [[nodiscard]] std::size_t getOffset() const noexcept;
        [[nodiscard]] std::size_t getLength() const noexcept;
        [[nodiscard]] std::string_view getValue() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TOKEN_HPP
