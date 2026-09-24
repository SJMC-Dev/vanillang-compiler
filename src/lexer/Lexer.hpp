#ifndef VNLC_LEXER_HPP
#define VNLC_LEXER_HPP

#include "lexer/LexerMode.hpp"
#include "token/Token.hpp"

#include "util/TokenKindUtil.hpp"
#include <istream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>

namespace vnlc {
    class Lexer {
    private:
        LexerMode mode;
        std::stack<int> parenthesisCounterStack;
        int parenthesisCounter;

        static const std::unordered_map<std::string_view, TokenKind> keywords;

        std::istream& source;
        std::string currentLine;

        std::size_t offset;
        int line;
        int column;

        bool exhausted;

        void advance();
        bool readline();
        void collect(std::string& tokenValue);

        [[nodiscard]] bool blank() const;
        [[nodiscard]] bool number() const;
        [[nodiscard]] bool special() const;
        [[nodiscard]] bool newline() const;
        [[nodiscard]] bool eof() const;
        [[nodiscard]] bool separator() const;

        [[nodiscard]] int peek() const;
        [[nodiscard]] int peek(int offset) const;

        [[nodiscard]] Token processStartsWithBlank(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token processStartsWithNumber(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token processStartsWithSpecial(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token processStartsWithNewline(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token processStartsWithEof(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token processStartsWithIdentifier(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);

        [[nodiscard]] Token scanStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanFormatStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanRawStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset);

    public:
        explicit Lexer(std::istream& input);

        [[nodiscard]] bool hasNext() const;
        [[nodiscard]] Token next();

        friend bool TokenKindUtil::isGeneralizedIdentifier(TokenKind type);
    };
} // namespace vnlc

#endif // VNLC_LEXER_HPP
