#include "lexer/Lexer.hpp"
#include <gtest/gtest.h>
#include <sstream>

namespace vnlc {

    TEST(LexerTest, Keywords) {
        std::istringstream input("let func if case class this public\nreadonly none true false \n import lambda command");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::FUNC);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IF);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::CASE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::CLASS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::THIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::PUBLIC);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::READONLY);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NONE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::TRUE_LITERAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::FALSE_LITERAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IMPORT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LAMBDA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::COMMAND);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Identifiers) {
        std::istringstream input("foo bar onLoad onTick world Player Entity");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        for (int i = 0; i < 6; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, EmptyInput) {
        std::istringstream input("");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, OnlyWhitespace) {
        std::istringstream input("   \t  \n  \r\n\t");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, OnlyNewlines) {
        std::istringstream input("\n\n\n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, OnlyWhitespaceAndNewlines) {
        std::istringstream input(" \t \n \r\n\t \n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Operators) {
        std::istringstream input("+ += - -= * *= ** / /= = == != < <= > >= && || ?? ! & | ^ % ~ ? : . ?. !. , ; ( ) { } [ ] @a @e @x @public $( \\ `");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::PLUS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::PLUS_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::MINUS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::MINUS_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::ASTERISK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::ASTERISK_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOUBLE_ASTERISK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SLASH);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SLASH_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOUBLE_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::EXCLAMATION_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_ANGLE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_ANGLE_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_ANGLE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_ANGLE_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOUBLE_AMPERSAND);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOUBLE_PIPE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOUBLE_QUESTION);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::EXCLAMATION);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::AMPERSAND);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::PIPE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::CARET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::PERCENT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::TILDE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::QUESTION);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::COLON);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::QUESTION_DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::EXCLAMATION_DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::COMMA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SEMICOLON);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_BRACKET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_BRACKET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Numbers) {
        std::istringstream input("123 0.456 7.89e10 0x1A 0b1010 0o77 .2f .1211 1e-5 1.2e+3 .3e-2 2.e4d 12b 345s 123456789L 2376. 0x1.2 0b10.01 0o7.5 1e+f 2e-3.5 4.5e6.7 8.9e+ 10.e-");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NUMBER);
        for (int i = 0; i < 15; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::NUMBER);
        }
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Comments) {
        std::istringstream input("# This is a single-line comment\n#* This is a \nmulti-line comment *#\n#* This is an unterminated multi-line comment\n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SINGLE_LINE_COMMENT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::MULTI_LINE_COMMENT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Strings) {
        std::istringstream input(
            "\"This is a string with a newline\\n and a tab\\t\"\n\"Another string with a single quote ' and a backslash\\\\\"\n\"Unterminated string\n'Another unterminated string'"
        );
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, RawStrings) {
        std::istringstream input("r\"This is a raw string with literal \\n and \\t\"\nR\"Another raw string with a single quote ' and two backslashs\\\\\"\nr\"Unterminated raw string\n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, FormatStrings) {
        std::istringstream input(
            "f\"This is a format string with an expression $(1 + 2)\"\nf\"Another format string with an expression $(player.name) and a nested expression "
            "$(world.print(f\"Hello, $(player.name)!\", @a))\"\nf\"Unterminated format string"
        );
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NUMBER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::PLUS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NUMBER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::COMMA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Characters) {
        std::istringstream input("'a' '\\n' '\\t' '\\'' '\\\\' 'ab' ''");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::CHAR);
        for (int i = 0; i < 4; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::CHAR);
        }
        for (int i = 0; i < 2; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, CodeExample) {
        std::istringstream input("func onLoad() {\n    world.print(\"Hello, Vanillang!\", @a)\n}");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::FUNC);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::COMMA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::RIGHT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, InvalidTokens) {
        std::istringstream input("@ @1 0xG 0b102 0o8 1.2.3 \"Unterminated string\n'Unterminated char");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        for (int i = 0; i < 6; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getKind(), TokenKind::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

} // namespace vnlc
