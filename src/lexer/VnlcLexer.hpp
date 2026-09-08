#ifndef VNLC_LEXER_HPP
#define VNLC_LEXER_HPP

#include "lexer/VnlcLexerMode.hpp"
#include "token/VnlcToken.hpp"

#include "util/VnlcTokenTypeUtil.hpp"
#include <istream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>

class VnlcLexer {
private:
    VnlcLexerMode mode;
    std::stack<int> parenthesisCounterStack;
    int parenthesisCounter;

    static const std::unordered_map<std::string_view, VnlcTokenType> keywords;

    std::istream& source;
    std::string currentLine;

    unsigned int offset;
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

    [[nodiscard]] VnlcToken processStartsWithBlank(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);
    [[nodiscard]] VnlcToken processStartsWithNumber(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);
    [[nodiscard]] VnlcToken processStartsWithSpecial(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);
    [[nodiscard]] VnlcToken processStartsWithNewline(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);
    [[nodiscard]] VnlcToken processStartsWithEof(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);
    [[nodiscard]] VnlcToken processStartsWithIdentifier(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);

    [[nodiscard]] VnlcToken scanStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);
    [[nodiscard]] VnlcToken scanFormatStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);
    [[nodiscard]] VnlcToken scanRawStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset);

public:
    explicit VnlcLexer(std::istream& input);

    [[nodiscard]] bool hasNext() const;
    [[nodiscard]] VnlcToken next();

    friend bool VnlcTokenTypeUtil::isGeneralizedIdentifier(VnlcTokenType type);
};

#endif // VNLC_LEXER_HPP
