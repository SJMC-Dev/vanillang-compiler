#include "Lexer.hpp"
#include "error/OutOfRangeError.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    const std::unordered_map<std::string_view, TokenKind> Lexer::keywords{
        { "var", TokenKind::VAR },
        { "let", TokenKind::LET },
        { "const", TokenKind::CONST },
        { "byte", TokenKind::BYTE_TYPE },
        { "short", TokenKind::SHORT_TYPE },
        { "int", TokenKind::INT_TYPE },
        { "long", TokenKind::LONG_TYPE },
        { "float", TokenKind::FLOAT_TYPE },
        { "double", TokenKind::DOUBLE_TYPE },
        { "bool", TokenKind::BOOL_TYPE },
        { "string", TokenKind::STRING_TYPE },
        { "func", TokenKind::FUNC },
        { "init", TokenKind::INIT },
        { "return", TokenKind::RETURN },
        { "if", TokenKind::IF },
        { "else", TokenKind::ELSE },
        { "for", TokenKind::FOR },
        { "while", TokenKind::WHILE },
        { "label", TokenKind::LABEL },
        { "break", TokenKind::BREAK },
        { "continue", TokenKind::CONTINUE },
        { "switch", TokenKind::SWITCH },
        { "case", TokenKind::CASE },
        { "default", TokenKind::DEFAULT },
        { "when", TokenKind::WHEN },
        { "context", TokenKind::CONTEXT },
        { "void", TokenKind::VOID },
        { "native", TokenKind::NATIVE },
        { "callee", TokenKind::CALLEE },
        { "in", TokenKind::IN },
        { "reload", TokenKind::RELOAD },
        { "class", TokenKind::CLASS },
        { "interface", TokenKind::INTERFACE },
        { "abstract", TokenKind::ABSTRACT },
        { "type", TokenKind::TYPE },
        { "enum", TokenKind::ENUM },
        { "record", TokenKind::RECORD },
        { "extends", TokenKind::EXTENDS },
        { "implements", TokenKind::IMPLEMENTS },
        { "this", TokenKind::THIS },
        { "super", TokenKind::SUPER },
        { "private", TokenKind::PRIVATE },
        { "protected", TokenKind::PROTECTED },
        { "public", TokenKind::PUBLIC },
        { "readonly", TokenKind::READONLY },
        { "static", TokenKind::STATIC },
        { "instanceof", TokenKind::INSTANCEOF },
        { "final", TokenKind::FINAL },
        { "override", TokenKind::OVERRIDE },
        { "none", TokenKind::NONE },
        { "true", TokenKind::TRUE },
        { "false", TokenKind::FALSE },
        { "import", TokenKind::IMPORT },
        { "export", TokenKind::EXPORT },
        { "as", TokenKind::AS },
        { "self", TokenKind::SELF },
        { "lambda", TokenKind::LAMBDA },
        { "command", TokenKind::COMMAND },
        { "async", TokenKind::ASYNC },
        { "await", TokenKind::AWAIT },
        { "generator", TokenKind::GENERATOR },
        { "yield", TokenKind::YIELD },
        { "from", TokenKind::FROM },
        { "decorator", TokenKind::DECORATOR },
        { "decorate", TokenKind::DECORATE },
        { "panic", TokenKind::PANIC },
        { "assert", TokenKind::ASSERT },
        { "typeof", TokenKind::TYPEOF },
        { "defer", TokenKind::DEFER },
        { "asset", TokenKind::ASSET },
        { "define", TokenKind::DEFINE },
        { "metadata", TokenKind::METADATA },
    };

    Lexer::Lexer(std::istream& input)
        : mode(LexerMode::DEFAULT),
          parenthesisCounterStack(),
          parenthesisCounter(-1),
          source(input),
          currentLine(""),
          offset(0),
          line(-1),
          column(0),
          exhausted(false) {
        readline();
    }

    int Lexer::peek() const {
        return peek(0);
    }

    int Lexer::peek(int offset) const {
        int targetColumn = column + offset;
        if (targetColumn < static_cast<int>(currentLine.length())) {
            return static_cast<char>(currentLine[targetColumn]);
        }

        return std::char_traits<char>::eof();
    }

    bool Lexer::blank() const {
        if (eof()) {
            return false;
        }
        char ch = static_cast<char>(peek());
        return ch == ' ' || ch == '\t' || ch == '\r';
    }

    bool Lexer::number() const {
        if (eof()) {
            return false;
        }
        char ch = static_cast<char>(peek());
        return ch >= '0' && ch <= '9';
    }

    bool Lexer::special() const {
        if (eof()) {
            return false;
        }
        char c = static_cast<char>(peek());
        constexpr std::string_view specialChars = "+-*/=!<>|&^%~?:.,;(){}[]#@$\\`\"'";
        return specialChars.find(c) != std::string_view::npos;
    }

    bool Lexer::newline() const {
        if (eof()) {
            return false;
        }
        char ch = static_cast<char>(peek());
        return ch == '\n';
    }

    bool Lexer::eof() const {
        return peek() == std::char_traits<char>::eof();
    }

    bool Lexer::separator() const {
        return eof() || blank() || special() || newline();
    }

    bool Lexer::readline() {
        currentLine.clear();

        int ch = source.get();
        if (ch == std::char_traits<char>::eof()) {
            return false;
        }

        line++;

        while (true) {
            currentLine.push_back(static_cast<char>(ch));
            if (ch == '\n') {
                break;
            }

            ch = source.get();
            if (ch == std::char_traits<char>::eof()) {
                break;
            }
        }

        column = 0;
        return true;
    }

    void Lexer::advance() {
        if (exhausted) {
            throw OutOfRangeError("No more tokens to read.");
        }

        offset++;

        if (eof()) {
            exhausted = true;
            return;
        }

        int len = static_cast<int>(currentLine.length());

        if (column < len - 1) {
            column++;
            return;
        }

        if (readline()) {
            return;
        }

        column = len;
    }

    void Lexer::collect(std::string& tokenValue) {
        char c = static_cast<char>(peek());
        tokenValue.push_back(c);
        advance();
    }

    bool Lexer::hasNext() const {
        return !exhausted;
    }

    Token Lexer::processStartsWithBlank(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        while (blank()) {
            if (peek() != '\r') {
                collect(tokenValue);
            } else {
                advance();
            }
        }

        return Token(TokenKind::BLANK, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    Token Lexer::processStartsWithNumber(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        constexpr unsigned char BIN = 0b001;
        constexpr unsigned char OCT = 0b010;
        constexpr unsigned char HEX = 0b100;

        bool existE = false;
        bool existDot = mode == LexerMode::NUMBER_STARTS_WITH_DOT;
        bool nonDecimal = false;
        unsigned char baseFlags = 0;

        mode = LexerMode::DEFAULT;

        std::string_view hexDigits = "0123456789abcdefABCDEF";
        std::string_view octalDigits = "01234567";
        std::string_view binaryDigits = "01";
        std::string_view nonDecimalFlags = "xob";
        std::string_view exponentFlags = "eE";
        std::string_view integerSuffixes = "bBsSlL";
        std::string_view floatSuffixes = "fFdD";

        while (true) {
            if (peek() != '.' && separator()) {
                break;
            }

            if (peek() == '0' && nonDecimalFlags.find(peek(1)) != std::string_view::npos) {
                nonDecimal = true;
                collect(tokenValue);
                baseFlags = peek() == 'b' ? BIN : peek() == 'o' ? OCT : peek() == 'x' ? HEX : 0;
                collect(tokenValue);

                if (separator() && peek() != '.') {
                    return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }

                continue;
            }

            if (peek() == '.') {
                if (peek(1) == '.') {
                    break;
                }

                collect(tokenValue);

                if (nonDecimal || existE || existDot) {
                    while (!separator()) {
                        collect(tokenValue);
                    }

                    return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }

                existDot = true;
                continue;
            }

            if (nonDecimal) {
                bool error = false;
                error |= baseFlags == BIN && binaryDigits.find(peek()) == std::string_view::npos;
                error |= baseFlags == OCT && octalDigits.find(peek()) == std::string_view::npos;
                error |= baseFlags == HEX && hexDigits.find(peek()) == std::string_view::npos;

                if (error) {
                    while (!separator()) {
                        collect(tokenValue);
                    }

                    return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            };

            if (!nonDecimal && exponentFlags.find(peek()) != std::string_view::npos) {
                collect(tokenValue);

                if (existE) {
                    while (!separator()) {
                        collect(tokenValue);
                    }

                    return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }

                if (peek() == '+' || peek() == '-') {
                    collect(tokenValue);

                    if (!number()) {
                        while (!separator()) {
                            collect(tokenValue);
                        }

                        return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                    }
                } else if (!number()) {
                    while (!separator()) {
                        collect(tokenValue);
                    }

                    return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }

                collect(tokenValue);
                existE = true;
                continue;
            }

            if (!nonDecimal && !number()) {
                if (existDot || existE) {
                    if (floatSuffixes.find(peek()) != std::string_view::npos) {
                        collect(tokenValue);
                        break;
                    } else {
                        while (!separator()) {
                            collect(tokenValue);
                        }

                        return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                    }
                } else {
                    if (integerSuffixes.find(peek()) != std::string_view::npos) {
                        collect(tokenValue);
                        break;
                    } else {
                        while (!separator()) {
                            collect(tokenValue);
                        }

                        return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                    }
                }
            }

            collect(tokenValue);
        }

        return Token(TokenKind::NUMBER, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    Token Lexer::processStartsWithSpecial(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        if (peek() == '+') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::PLUS_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '+') {
                collect(tokenValue);
                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::PLUS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '-') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::MINUS_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '-') {
                collect(tokenValue);
                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '>') {
                collect(tokenValue);
                return Token(TokenKind::ARROW, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::MINUS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '*') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::ASTERISK_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '*') {
                collect(tokenValue);

                if (peek() == '=') {
                    collect(tokenValue);
                    return Token(TokenKind::DOUBLE_ASTERISK_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                } else {
                    return Token(TokenKind::DOUBLE_ASTERISK, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else {
                return Token(TokenKind::ASTERISK, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '/') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::SLASH_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '/') {
                collect(tokenValue);

                if (peek() == '=') {
                    collect(tokenValue);
                    return Token(TokenKind::DOUBLE_SLASH_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                } else {
                    return Token(TokenKind::DOUBLE_SLASH, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else {
                return Token(TokenKind::SLASH, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '%') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::PERCENT_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::PERCENT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '<') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::LEFT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '<') {
                collect(tokenValue);

                if (peek() == '=') {
                    collect(tokenValue);
                    return Token(TokenKind::DOUBLE_LEFT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                } else {
                    return Token(TokenKind::DOUBLE_LEFT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else {
                return Token(TokenKind::LEFT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '>') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::RIGHT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '>') {
                collect(tokenValue);

                if (peek() == '=') {
                    collect(tokenValue);
                    return Token(TokenKind::DOUBLE_RIGHT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                } else if (peek() == '>') {
                    collect(tokenValue);

                    if (peek() == '=') {
                        collect(tokenValue);
                        return Token(TokenKind::TRIPLE_RIGHT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                    } else {
                        return Token(TokenKind::TRIPLE_RIGHT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                    }
                } else {
                    return Token(TokenKind::DOUBLE_RIGHT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else {
                return Token(TokenKind::RIGHT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '=') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::DOUBLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '?') {
            collect(tokenValue);

            if (peek() == '?') {
                collect(tokenValue);

                if (peek() == '=') {
                    collect(tokenValue);
                    return Token(TokenKind::DOUBLE_QUESTION_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                } else {
                    return Token(TokenKind::DOUBLE_QUESTION, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else if (peek() == '.') {
                collect(tokenValue);
                return Token(TokenKind::QUESTION_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::QUESTION, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '!') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::EXCLAMATION_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '.') {
                collect(tokenValue);
                return Token(TokenKind::EXCLAMATION_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::EXCLAMATION, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '&') {
            collect(tokenValue);

            if (peek() == '&') {
                collect(tokenValue);
                return Token(TokenKind::DOUBLE_AMPERSAND, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::AMPERSAND_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::AMPERSAND, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '|') {
            collect(tokenValue);

            if (peek() == '|') {
                collect(tokenValue);
                return Token(TokenKind::DOUBLE_PIPE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::PIPE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::PIPE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '^') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return Token(TokenKind::CARET_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::CARET, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '~') {
            collect(tokenValue);
            return Token(TokenKind::TILDE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '.') {
            collect(tokenValue);

            if (peek() == '.') {
                collect(tokenValue);

                if (peek() == '.') {
                    collect(tokenValue);
                    return Token(TokenKind::TRIPLE_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                } else {
                    return Token(TokenKind::DOUBLE_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else if (number()) {
                mode = LexerMode::NUMBER_STARTS_WITH_DOT;
                return processStartsWithNumber(tokenValue, currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == ',') {
            collect(tokenValue);
            return Token(TokenKind::COMMA, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == ';') {
            collect(tokenValue);
            return Token(TokenKind::SEMICOLON, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == ':') {
            collect(tokenValue);

            if (peek() == ':') {
                collect(tokenValue);
                return Token(TokenKind::DOUBLE_COLON, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return Token(TokenKind::COLON, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '(') {
            collect(tokenValue);

            if (parenthesisCounter >= 0) {
                parenthesisCounter++;
            }

            return Token(TokenKind::LEFT_PARENTHESIS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == ')') {
            collect(tokenValue);

            if (parenthesisCounter > 0) {
                parenthesisCounter--;
                if (parenthesisCounter == 0) {
                    parenthesisCounter = parenthesisCounterStack.top();
                    parenthesisCounterStack.pop();

                    mode = LexerMode::FORMAT_STRING;
                }
            }

            return Token(TokenKind::RIGHT_PARENTHESIS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '{') {
            collect(tokenValue);
            return Token(TokenKind::LEFT_BRACE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '}') {
            collect(tokenValue);
            return Token(TokenKind::RIGHT_BRACE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '[') {
            collect(tokenValue);
            return Token(TokenKind::LEFT_BRACKET, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == ']') {
            collect(tokenValue);
            return Token(TokenKind::RIGHT_BRACKET, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '#') {
            collect(tokenValue);

            if (peek() == '*') {
                collect(tokenValue);

                while (true) {
                    if (eof()) {
                        return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                    }

                    if (peek() == '*' && peek(1) == '#') {
                        collect(tokenValue);
                        collect(tokenValue);
                        break;
                    }

                    collect(tokenValue);
                }

                return Token(TokenKind::MULTI_LINE_COMMENT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                while (!eof() && !newline() && peek() != '\r') {
                    collect(tokenValue);
                }

                return Token(TokenKind::SINGLE_LINE_COMMENT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '@') {
            collect(tokenValue);

            std::string_view selectorPrefixes = "praesn";

            if (selectorPrefixes.find(peek()) != std::string_view::npos) {
                collect(tokenValue);

                if (!separator()) {
                    while (!separator()) {
                        collect(tokenValue);
                    }

                    return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }

                return Token(TokenKind::SELECTOR_PREFIX, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                while (!separator()) {
                    collect(tokenValue);
                }

                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '$') {
            collect(tokenValue);

            if (mode == LexerMode::INTERPOLATION_BEGIN && peek() == '(') {
                collect(tokenValue);

                parenthesisCounterStack.push(parenthesisCounter);
                parenthesisCounter = 1;
                mode = LexerMode::DEFAULT;

                return Token(TokenKind::INTERPOLATION_START, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                while (!separator()) {
                    collect(tokenValue);
                }

                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '\'') {
            collect(tokenValue);

            bool error = false;
            int charCount = 0;
            std::string_view escapeChars = "0bfnrst\\\"'";

            while (true) {
                if (eof() || newline() || peek() == '\r') {
                    return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }

                if (peek() == '\\') {
                    collect(tokenValue);

                    if (escapeChars.find(peek()) == std::string_view::npos) {
                        error = true;
                    }

                    collect(tokenValue);
                    charCount++;
                    continue;
                }

                if (peek() == '\'') {
                    collect(tokenValue);
                    break;
                }

                collect(tokenValue);
                charCount++;
            }

            error |= (charCount != 1);

            if (error) {
                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }

            return Token(TokenKind::CHAR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '"') {
            collect(tokenValue);
            mode = LexerMode::STRING;
            return scanStringLiteral(tokenValue, currentLine, currentColumn, currentOffset);
        } else {
            collect(tokenValue);

            while (!separator()) {
                collect(tokenValue);
            }

            return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    }

    Token Lexer::processStartsWithNewline(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        collect(tokenValue);
        return Token(TokenKind::NEWLINE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    Token Lexer::processStartsWithEof(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        advance();
        return Token(TokenKind::END_OF_FILE, "", currentLine, currentColumn, currentOffset);
    }

    Token Lexer::processStartsWithIdentifier(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        bool error = false;

        if ((peek() == 'f' || peek() == 'F') && peek(1) == '"') {
            collect(tokenValue);
            collect(tokenValue);

            mode = LexerMode::FORMAT_STRING;
            return scanFormatStringLiteral(tokenValue, currentLine, currentColumn, currentOffset);
        }

        if ((peek() == 'r' || peek() == 'R') && peek(1) == '"') {
            collect(tokenValue);
            collect(tokenValue);

            mode = LexerMode::RAW_STRING;
            return scanRawStringLiteral(tokenValue, currentLine, currentColumn, currentOffset);
        }

        while (!eof() && !blank() && !special() && !newline()) {
            if (peek() >= 0 && peek() < 32) {
                collect(tokenValue);
                error = true;
                break;
            }

            collect(tokenValue);
        }

        if (error) {
            while (!separator()) {
                collect(tokenValue);
            }

            return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (keywords.find(tokenValue) != keywords.end()) {
            return Token(keywords.at(tokenValue), std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return Token(TokenKind::IDENTIFIER, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    }

    Token Lexer::scanStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        bool error = false;
        std::string_view escapeChars = "bfnrst\\\"'";

        while (true) {
            if (eof() || newline()) {
                mode = LexerMode::DEFAULT;
                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }

            if (peek() == '\\') {
                collect(tokenValue);

                if (escapeChars.find(peek()) == std::string_view::npos) {
                    error = true;
                }

                collect(tokenValue);
                continue;
            }

            if (peek() == '"') {
                collect(tokenValue);
                break;
            }

            collect(tokenValue);
        }

        mode = LexerMode::DEFAULT;

        if (error) {
            return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }

        return Token(TokenKind::STRING, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    Token Lexer::scanFormatStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        bool error = false;
        std::string_view escapeChars = "bfnrst\\\"'$";

        while (true) {
            if (eof() || newline()) {
                mode = LexerMode::DEFAULT;
                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }

            if (peek() == '\\') {
                collect(tokenValue);

                if (escapeChars.find(peek()) == std::string_view::npos) {
                    error = true;
                }

                collect(tokenValue);
                continue;
            }

            if (peek() == '"') {
                collect(tokenValue);
                mode = LexerMode::DEFAULT;
                break;
            }

            if (peek() == '$' && peek(1) == '(') {
                mode = LexerMode::INTERPOLATION_BEGIN;
                break;
            }

            collect(tokenValue);
        }

        if (error) {
            return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }

        return Token(TokenKind::STRING, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    Token Lexer::scanRawStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, std::size_t currentOffset) {
        while (true) {
            if (eof() || newline()) {
                mode = LexerMode::DEFAULT;
                return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }

            if (peek() == '"') {
                collect(tokenValue);
                break;
            }

            collect(tokenValue);
        }

        mode = LexerMode::DEFAULT;
        return Token(TokenKind::STRING, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    Token Lexer::next() {
        if (!hasNext()) {
            throw OutOfRangeError("No more tokens to read.");
        }

        std::string tokenValue;
        int currentOffset = offset;
        int currentLine = line + 1;
        int currentColumn = column + 1;

        if (mode == LexerMode::FORMAT_STRING) {
            return scanFormatStringLiteral(tokenValue, currentLine, currentColumn, currentOffset);
        } else {
            if (eof()) {
                return processStartsWithEof(tokenValue, currentLine, currentColumn, currentOffset);
            } else if (blank()) {
                return processStartsWithBlank(tokenValue, currentLine, currentColumn, currentOffset);
            } else if (newline()) {
                return processStartsWithNewline(tokenValue, currentLine, currentColumn, currentOffset);
            } else if (special()) {
                return processStartsWithSpecial(tokenValue, currentLine, currentColumn, currentOffset);
            } else if (number()) {
                return processStartsWithNumber(tokenValue, currentLine, currentColumn, currentOffset);
            } else {
                return processStartsWithIdentifier(tokenValue, currentLine, currentColumn, currentOffset);
            }
        }

        return Token(TokenKind::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }
} // namespace vnlc
