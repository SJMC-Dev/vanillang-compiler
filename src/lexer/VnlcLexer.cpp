#include "VnlcLexer.hpp"
#include "error/VnlcOutOfRangeError.hpp"
#include <string>
#include <string_view>

const std::unordered_map<std::string_view, VnlcTokenType> VnlcLexer::keywords{
    { "var", VnlcTokenType::VAR },
    { "let", VnlcTokenType::LET },
    { "const", VnlcTokenType::CONST },
    { "byte", VnlcTokenType::BYTE_TYPE },
    { "short", VnlcTokenType::SHORT_TYPE },
    { "int", VnlcTokenType::INT_TYPE },
    { "long", VnlcTokenType::LONG_TYPE },
    { "float", VnlcTokenType::FLOAT_TYPE },
    { "double", VnlcTokenType::DOUBLE_TYPE },
    { "bool", VnlcTokenType::BOOL_TYPE },
    { "string", VnlcTokenType::STRING_TYPE },
    { "func", VnlcTokenType::FUNC },
    { "init", VnlcTokenType::INIT },
    { "return", VnlcTokenType::RETURN },
    { "if", VnlcTokenType::IF },
    { "else", VnlcTokenType::ELSE },
    { "for", VnlcTokenType::FOR },
    { "while", VnlcTokenType::WHILE },
    { "label", VnlcTokenType::LABEL },
    { "break", VnlcTokenType::BREAK },
    { "continue", VnlcTokenType::CONTINUE },
    { "switch", VnlcTokenType::SWITCH },
    { "case", VnlcTokenType::CASE },
    { "default", VnlcTokenType::DEFAULT },
    { "when", VnlcTokenType::WHEN },
    { "context", VnlcTokenType::CONTEXT },
    { "void", VnlcTokenType::VOID },
    { "native", VnlcTokenType::NATIVE },
    { "callee", VnlcTokenType::CALLEE },
    { "in", VnlcTokenType::IN },
    { "reload", VnlcTokenType::RELOAD },
    { "class", VnlcTokenType::CLASS },
    { "interface", VnlcTokenType::INTERFACE },
    { "abstract", VnlcTokenType::ABSTRACT },
    { "type", VnlcTokenType::TYPE },
    { "enum", VnlcTokenType::ENUM },
    { "record", VnlcTokenType::RECORD },
    { "extends", VnlcTokenType::EXTENDS },
    { "implements", VnlcTokenType::IMPLEMENTS },
    { "this", VnlcTokenType::THIS },
    { "super", VnlcTokenType::SUPER },
    { "private", VnlcTokenType::PRIVATE },
    { "protected", VnlcTokenType::PROTECTED },
    { "public", VnlcTokenType::PUBLIC },
    { "readonly", VnlcTokenType::READONLY },
    { "static", VnlcTokenType::STATIC },
    { "instanceof", VnlcTokenType::INSTANCEOF },
    { "final", VnlcTokenType::FINAL },
    { "override", VnlcTokenType::OVERRIDE },
    { "none", VnlcTokenType::NONE },
    { "true", VnlcTokenType::TRUE },
    { "false", VnlcTokenType::FALSE },
    { "import", VnlcTokenType::IMPORT },
    { "export", VnlcTokenType::EXPORT },
    { "as", VnlcTokenType::AS },
    { "self", VnlcTokenType::SELF },
    { "lambda", VnlcTokenType::LAMBDA },
    { "command", VnlcTokenType::COMMAND },
    { "async", VnlcTokenType::ASYNC },
    { "await", VnlcTokenType::AWAIT },
    { "generator", VnlcTokenType::GENERATOR },
    { "yield", VnlcTokenType::YIELD },
    { "from", VnlcTokenType::FROM },
    { "decorator", VnlcTokenType::DECORATOR },
    { "decorate", VnlcTokenType::DECORATE },
    { "panic", VnlcTokenType::PANIC },
    { "assert", VnlcTokenType::ASSERT },
    { "typeof", VnlcTokenType::TYPEOF },
    { "defer", VnlcTokenType::DEFER },
    { "asset", VnlcTokenType::ASSET },
    { "define", VnlcTokenType::DEFINE },
    { "metadata", VnlcTokenType::METADATA },
};

VnlcLexer::VnlcLexer(std::istream& input)
    : mode(VnlcLexerMode::DEFAULT),
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

int VnlcLexer::peek() const {
    return peek(0);
}

int VnlcLexer::peek(int offset) const {
    int targetColumn = column + offset;
    if (targetColumn < static_cast<int>(currentLine.length())) {
        return static_cast<char>(currentLine[targetColumn]);
    }

    return std::char_traits<char>::eof();
}

bool VnlcLexer::blank() const {
    if (eof()) {
        return false;
    }
    char ch = static_cast<char>(peek());
    return ch == ' ' || ch == '\t' || ch == '\r';
}

bool VnlcLexer::number() const {
    if (eof()) {
        return false;
    }
    char ch = static_cast<char>(peek());
    return ch >= '0' && ch <= '9';
}

bool VnlcLexer::special() const {
    if (eof()) {
        return false;
    }
    char c = static_cast<char>(peek());
    constexpr std::string_view specialChars = "+-*/=!<>|&^%~?:.,;(){}[]#@$\\`\"'";
    return specialChars.find(c) != std::string_view::npos;
}

bool VnlcLexer::newline() const {
    if (eof()) {
        return false;
    }
    char ch = static_cast<char>(peek());
    return ch == '\n';
}

bool VnlcLexer::eof() const {
    return peek() == std::char_traits<char>::eof();
}

bool VnlcLexer::separator() const {
    return eof() || blank() || special() || newline();
}

bool VnlcLexer::readline() {
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

void VnlcLexer::advance() {
    if (exhausted) {
        throw VnlcOutOfRangeError("No more tokens to read.");
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

void VnlcLexer::collect(std::string& tokenValue) {
    char c = static_cast<char>(peek());
    tokenValue.push_back(c);
    advance();
}

bool VnlcLexer::hasNext() const {
    return !exhausted;
}

VnlcToken VnlcLexer::processStartsWithBlank(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    while (blank()) {
        if (peek() != '\r') {
            collect(tokenValue);
        } else {
            advance();
        }
    }

    return VnlcToken(VnlcTokenType::BLANK, std::move(tokenValue), currentLine, currentColumn, currentOffset);
}

VnlcToken VnlcLexer::processStartsWithNumber(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    constexpr unsigned char BIN = 0b001;
    constexpr unsigned char OCT = 0b010;
    constexpr unsigned char HEX = 0b100;

    bool existE = false;
    bool existDot = mode == VnlcLexerMode::NUMBER_STARTS_WITH_DOT;
    bool nonDecimal = false;
    unsigned char baseFlags = 0;

    mode = VnlcLexerMode::DEFAULT;

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
                return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
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

                return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
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

                return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        };

        if (!nonDecimal && exponentFlags.find(peek()) != std::string_view::npos) {
            collect(tokenValue);

            if (existE) {
                while (!separator()) {
                    collect(tokenValue);
                }

                return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }

            if (peek() == '+' || peek() == '-') {
                collect(tokenValue);

                if (!number()) {
                    while (!separator()) {
                        collect(tokenValue);
                    }

                    return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else if (!number()) {
                while (!separator()) {
                    collect(tokenValue);
                }

                return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
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

                    return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else {
                if (integerSuffixes.find(peek()) != std::string_view::npos) {
                    collect(tokenValue);
                    break;
                } else {
                    while (!separator()) {
                        collect(tokenValue);
                    }

                    return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            }
        }

        collect(tokenValue);
    }

    return VnlcToken(VnlcTokenType::NUMBER, std::move(tokenValue), currentLine, currentColumn, currentOffset);
}

VnlcToken VnlcLexer::processStartsWithSpecial(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    if (peek() == '+') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::PLUS_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '+') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::PLUS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '-') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::MINUS_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '-') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '>') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::ARROW, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::MINUS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '*') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::ASTERISK_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '*') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return VnlcToken(VnlcTokenType::DOUBLE_ASTERISK_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return VnlcToken(VnlcTokenType::DOUBLE_ASTERISK, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else {
            return VnlcToken(VnlcTokenType::ASTERISK, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '/') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::SLASH_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '/') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return VnlcToken(VnlcTokenType::DOUBLE_SLASH_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return VnlcToken(VnlcTokenType::DOUBLE_SLASH, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else {
            return VnlcToken(VnlcTokenType::SLASH, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '%') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::PERCENT_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::PERCENT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '<') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::LEFT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '<') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return VnlcToken(VnlcTokenType::DOUBLE_LEFT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return VnlcToken(VnlcTokenType::DOUBLE_LEFT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else {
            return VnlcToken(VnlcTokenType::LEFT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '>') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::RIGHT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '>') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return VnlcToken(VnlcTokenType::DOUBLE_RIGHT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else if (peek() == '>') {
                collect(tokenValue);

                if (peek() == '=') {
                    collect(tokenValue);
                    return VnlcToken(VnlcTokenType::TRIPLE_RIGHT_ANGLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                } else {
                    return VnlcToken(VnlcTokenType::TRIPLE_RIGHT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }
            } else {
                return VnlcToken(VnlcTokenType::DOUBLE_RIGHT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else {
            return VnlcToken(VnlcTokenType::RIGHT_ANGLE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '=') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::DOUBLE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '?') {
        collect(tokenValue);

        if (peek() == '?') {
            collect(tokenValue);

            if (peek() == '=') {
                collect(tokenValue);
                return VnlcToken(VnlcTokenType::DOUBLE_QUESTION_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return VnlcToken(VnlcTokenType::DOUBLE_QUESTION, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (peek() == '.') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::QUESTION_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::QUESTION, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '!') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::EXCLAMATION_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '.') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::EXCLAMATION_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::EXCLAMATION, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '&') {
        collect(tokenValue);

        if (peek() == '&') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::DOUBLE_AMPERSAND, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::AMPERSAND_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::AMPERSAND, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '|') {
        collect(tokenValue);

        if (peek() == '|') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::DOUBLE_PIPE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::PIPE_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::PIPE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '^') {
        collect(tokenValue);

        if (peek() == '=') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::CARET_EQUAL, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::CARET, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '~') {
        collect(tokenValue);
        return VnlcToken(VnlcTokenType::TILDE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == '.') {
        collect(tokenValue);

        if (peek() == '.') {
            collect(tokenValue);

            if (peek() == '.') {
                collect(tokenValue);
                return VnlcToken(VnlcTokenType::TRIPLE_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            } else {
                return VnlcToken(VnlcTokenType::DOUBLE_DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }
        } else if (number()) {
            mode = VnlcLexerMode::NUMBER_STARTS_WITH_DOT;
            return processStartsWithNumber(tokenValue, currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::DOT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == ',') {
        collect(tokenValue);
        return VnlcToken(VnlcTokenType::COMMA, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == ';') {
        collect(tokenValue);
        return VnlcToken(VnlcTokenType::SEMICOLON, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == ':') {
        collect(tokenValue);

        if (peek() == ':') {
            collect(tokenValue);
            return VnlcToken(VnlcTokenType::DOUBLE_COLON, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            return VnlcToken(VnlcTokenType::COLON, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '(') {
        collect(tokenValue);

        if (parenthesisCounter >= 0) {
            parenthesisCounter++;
        }

        return VnlcToken(VnlcTokenType::LEFT_PARENTHESIS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == ')') {
        collect(tokenValue);

        if (parenthesisCounter > 0) {
            parenthesisCounter--;
            if (parenthesisCounter == 0) {
                parenthesisCounter = parenthesisCounterStack.top();
                parenthesisCounterStack.pop();

                mode = VnlcLexerMode::FORMAT_STRING;
            }
        }

        return VnlcToken(VnlcTokenType::RIGHT_PARENTHESIS, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == '{') {
        collect(tokenValue);
        return VnlcToken(VnlcTokenType::LEFT_BRACE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == '}') {
        collect(tokenValue);
        return VnlcToken(VnlcTokenType::RIGHT_BRACE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == '[') {
        collect(tokenValue);
        return VnlcToken(VnlcTokenType::LEFT_BRACKET, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == ']') {
        collect(tokenValue);
        return VnlcToken(VnlcTokenType::RIGHT_BRACKET, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == '#') {
        collect(tokenValue);

        if (peek() == '*') {
            collect(tokenValue);

            while (true) {
                if (eof()) {
                    return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
                }

                if (peek() == '*' && peek(1) == '#') {
                    collect(tokenValue);
                    collect(tokenValue);
                    break;
                }

                collect(tokenValue);
            }

            return VnlcToken(VnlcTokenType::MULTI_LINE_COMMENT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            while (!eof() && !newline() && peek() != '\r') {
                collect(tokenValue);
            }

            return VnlcToken(VnlcTokenType::SINGLE_LINE_COMMENT, std::move(tokenValue), currentLine, currentColumn, currentOffset);
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

                return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
            }

            return VnlcToken(VnlcTokenType::SELECTOR_PREFIX, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            while (!separator()) {
                collect(tokenValue);
            }

            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '$') {
        collect(tokenValue);

        if (mode == VnlcLexerMode::INTERPOLATION_BEGIN && peek() == '(') {
            collect(tokenValue);

            parenthesisCounterStack.push(parenthesisCounter);
            parenthesisCounter = 1;
            mode = VnlcLexerMode::DEFAULT;

            return VnlcToken(VnlcTokenType::INTERPOLATION_START, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        } else {
            while (!separator()) {
                collect(tokenValue);
            }

            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }
    } else if (peek() == '\'') {
        collect(tokenValue);

        bool error = false;
        int charCount = 0;
        std::string_view escapeChars = "0bfnrst\\\"'";

        while (true) {
            if (eof() || newline() || peek() == '\r') {
                return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
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
            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }

        return VnlcToken(VnlcTokenType::CHAR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (peek() == '"') {
        collect(tokenValue);
        mode = VnlcLexerMode::STRING;
        return scanStringLiteral(tokenValue, currentLine, currentColumn, currentOffset);
    } else {
        collect(tokenValue);

        while (!separator()) {
            collect(tokenValue);
        }

        return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }
}

VnlcToken VnlcLexer::processStartsWithNewline(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    collect(tokenValue);
    return VnlcToken(VnlcTokenType::NEWLINE, std::move(tokenValue), currentLine, currentColumn, currentOffset);
}

VnlcToken VnlcLexer::processStartsWithEof(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    advance();
    return VnlcToken(VnlcTokenType::END_OF_FILE, "", currentLine, currentColumn, currentOffset);
}

VnlcToken VnlcLexer::processStartsWithIdentifier(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    bool error = false;

    if ((peek() == 'f' || peek() == 'F') && peek(1) == '"') {
        collect(tokenValue);
        collect(tokenValue);

        mode = VnlcLexerMode::FORMAT_STRING;
        return scanFormatStringLiteral(tokenValue, currentLine, currentColumn, currentOffset);
    }

    if ((peek() == 'r' || peek() == 'R') && peek(1) == '"') {
        collect(tokenValue);
        collect(tokenValue);

        mode = VnlcLexerMode::RAW_STRING;
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

        return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else if (keywords.find(tokenValue) != keywords.end()) {
        return VnlcToken(keywords.at(tokenValue), std::move(tokenValue), currentLine, currentColumn, currentOffset);
    } else {
        return VnlcToken(VnlcTokenType::IDENTIFIER, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }
}

VnlcToken VnlcLexer::scanStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    bool error = false;
    std::string_view escapeChars = "bfnrst\\\"'";

    while (true) {
        if (eof() || newline()) {
            mode = VnlcLexerMode::DEFAULT;
            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
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

    mode = VnlcLexerMode::DEFAULT;

    if (error) {
        return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    return VnlcToken(VnlcTokenType::STRING, std::move(tokenValue), currentLine, currentColumn, currentOffset);
}

VnlcToken VnlcLexer::scanFormatStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    bool error = false;
    std::string_view escapeChars = "bfnrst\\\"'$";

    while (true) {
        if (eof() || newline()) {
            mode = VnlcLexerMode::DEFAULT;
            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
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
            mode = VnlcLexerMode::DEFAULT;
            break;
        }

        if (peek() == '$' && peek(1) == '(') {
            mode = VnlcLexerMode::INTERPOLATION_BEGIN;
            break;
        }

        collect(tokenValue);
    }

    if (error) {
        return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
    }

    return VnlcToken(VnlcTokenType::STRING, std::move(tokenValue), currentLine, currentColumn, currentOffset);
}

VnlcToken VnlcLexer::scanRawStringLiteral(std::string& tokenValue, int currentLine, int currentColumn, unsigned int currentOffset) {
    while (true) {
        if (eof() || newline()) {
            mode = VnlcLexerMode::DEFAULT;
            return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
        }

        if (peek() == '"') {
            collect(tokenValue);
            break;
        }

        collect(tokenValue);
    }

    mode = VnlcLexerMode::DEFAULT;
    return VnlcToken(VnlcTokenType::STRING, std::move(tokenValue), currentLine, currentColumn, currentOffset);
}

VnlcToken VnlcLexer::next() {
    if (!hasNext()) {
        throw VnlcOutOfRangeError("No more tokens to read.");
    }

    std::string tokenValue;
    int currentOffset = offset;
    int currentLine = line + 1;
    int currentColumn = column + 1;

    if (mode == VnlcLexerMode::FORMAT_STRING) {
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

    return VnlcToken(VnlcTokenType::LEXICAL_ERROR, std::move(tokenValue), currentLine, currentColumn, currentOffset);
}