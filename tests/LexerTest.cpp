#include "lexer/Lexer.hpp"
#include <gtest/gtest.h>
#include <sstream>

namespace vnlc {

    TEST(LexerTest, Keywords) {
        std::istringstream input("let func if case class this public\nreadonly none true false \n import lambda command");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::FUNC);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IF);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::CASE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::CLASS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::THIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::PUBLIC);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::READONLY);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NONE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::TRUE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::FALSE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IMPORT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LAMBDA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::COMMAND);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Identifiers) {
        std::istringstream input("foo bar onLoad onTick world Player Entity");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        for (int i = 0; i < 6; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, EmptyInput) {
        std::istringstream input("");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, OnlyWhitespace) {
        std::istringstream input("   \t  \n  \r\n\t");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, OnlyNewlines) {
        std::istringstream input("\n\n\n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, OnlyWhitespaceAndNewlines) {
        std::istringstream input(" \t \n \r\n\t \n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Operators) {
        std::istringstream input("+ += - -= * *= ** / /= = == != < <= > >= && || ?? ! & | ^ % ~ ? : . ?. !. , ; ( ) { } [ ] @a @e @x @public $( \\ `");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::PLUS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::PLUS_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::MINUS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::MINUS_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::ASTERISK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::ASTERISK_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOUBLE_ASTERISK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SLASH);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SLASH_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOUBLE_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::EXCLAMATION_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_ANGLE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_ANGLE_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_ANGLE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_ANGLE_EQUAL);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOUBLE_AMPERSAND);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOUBLE_PIPE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOUBLE_QUESTION);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::EXCLAMATION);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::AMPERSAND);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::PIPE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::CARET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::PERCENT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::TILDE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::QUESTION);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::COLON);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::QUESTION_DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::EXCLAMATION_DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::COMMA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SEMICOLON);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_BRACKET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_BRACKET);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Numbers) {
        std::istringstream input("123 0.456 7.89e10 0x1A 0b1010 0o77 .2f .1211 1e-5 1.2e+3 .3e-2 2.e4d 12b 345s 123456789L 2376. 0x1.2 0b10.01 0o7.5 1e+f 2e-3.5 4.5e6.7 8.9e+ 10.e-");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NUMBER);
        for (int i = 0; i < 15; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::NUMBER);
        }
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Comments) {
        std::istringstream input("# This is a single-line comment\n#* This is a \nmulti-line comment *#\n#* This is an unterminated multi-line comment\n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SINGLE_LINE_COMMENT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::MULTI_LINE_COMMENT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Strings) {
        std::istringstream input(
            "\"This is a string with a newline\\n and a tab\\t\"\n\"Another string with a single quote ' and a backslash\\\\\"\n\"Unterminated string\n'Another unterminated string'"
        );
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, RawStrings) {
        std::istringstream input("r\"This is a raw string with literal \\n and \\t\"\nR\"Another raw string with a single quote ' and two backslashs\\\\\"\nr\"Unterminated raw string\n");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, FormatStrings) {
        std::istringstream input(
            "f\"This is a format string with an expression $(1 + 2)\"\nf\"Another format string with an expression $(player.name) and a nested expression "
            "$(world.print(f\"Hello, $(player.name)!\", @a))\"\nf\"Unterminated format string"
        );
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NUMBER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::PLUS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NUMBER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::INTERPOLATION_START);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::COMMA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, Characters) {
        std::istringstream input("'a' '\\n' '\\t' '\\'' '\\\\' 'ab' ''");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::CHAR);
        for (int i = 0; i < 4; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::CHAR);
        }
        for (int i = 0; i < 2; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, CodeExample) {
        std::istringstream input("func onLoad() {\n    world.print(\"Hello, Vanillang!\", @a)\n}");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::FUNC);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::DOT);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::IDENTIFIER);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEFT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::STRING);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::COMMA);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::SELECTOR_PREFIX);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_PARENTHESIS);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::RIGHT_BRACE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

    TEST(LexerTest, InvalidTokens) {
        std::istringstream input("@ @1 0xG 0b102 0o8 1.2.3 \"Unterminated string\n'Unterminated char");
        Lexer lexer(input);

        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        for (int i = 0; i < 6; i++) {
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::BLANK);
            ASSERT_TRUE(lexer.hasNext());
            ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        }
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::NEWLINE);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::LEXICAL_ERROR);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), TokenType::END_OF_FILE);
        ASSERT_FALSE(lexer.hasNext());
    }

} // namespace vnlc
