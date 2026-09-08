#include "lexer/VnlcLexer.hpp"
#include <gtest/gtest.h>
#include <sstream>

TEST(VnlcLexerTest, Keywords) {
    std::istringstream input("let func if case class this public\nreadonly none true false \n import lambda command");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LET);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::FUNC);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IF);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::CASE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::CLASS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::THIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::PUBLIC);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::READONLY);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NONE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::TRUE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::FALSE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IMPORT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LAMBDA);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::COMMAND);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, Identifiers) {
    std::istringstream input("foo bar onLoad onTick world Player Entity");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    for (int i = 0; i < 6; i++) {
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    }
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, EmptyInput) {
    std::istringstream input("");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, OnlyWhitespace) {
    std::istringstream input("   \t  \n  \r\n\t");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, OnlyNewlines) {
    std::istringstream input("\n\n\n");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, OnlyWhitespaceAndNewlines) {
    std::istringstream input(" \t \n \r\n\t \n");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, Operators) {
    std::istringstream input("+ += - -= * *= ** / /= = == != < <= > >= && || ?? ! & | ^ % ~ ? : . ?. !. , ; ( ) { } [ ] @a @e @x @public $( \\ `");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::PLUS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::PLUS_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::MINUS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::MINUS_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::ASTERISK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::ASTERISK_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOUBLE_ASTERISK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SLASH);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SLASH_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOUBLE_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::EXCLAMATION_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_ANGLE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_ANGLE_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_ANGLE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_ANGLE_EQUAL);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOUBLE_AMPERSAND);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOUBLE_PIPE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOUBLE_QUESTION);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::EXCLAMATION);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::AMPERSAND);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::PIPE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::CARET);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::PERCENT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::TILDE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::QUESTION);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::COLON);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::QUESTION_DOT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::EXCLAMATION_DOT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::COMMA);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SEMICOLON);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_BRACE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_BRACE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_BRACKET);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_BRACKET);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SELECTOR_PREFIX);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SELECTOR_PREFIX);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, Numbers) {
    std::istringstream input("123 0.456 7.89e10 0x1A 0b1010 0o77 .2f .1211 1e-5 1.2e+3 .3e-2 2.e4d 12b 345s 123456789L 2376. 0x1.2 0b10.01 0o7.5 1e+f 2e-3.5 4.5e6.7 8.9e+ 10.e-");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NUMBER);
    for (int i = 0; i < 15; i++) {
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NUMBER);
    }
    for (int i = 0; i < 8; i++) {
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    }
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, Comments) {
    std::istringstream input("# This is a single-line comment\n#* This is a \nmulti-line comment *#\n#* This is an unterminated multi-line comment\n");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SINGLE_LINE_COMMENT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::MULTI_LINE_COMMENT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, Strings) {
    std::istringstream input(
        "\"This is a string with a newline\\n and a tab\\t\"\n\"Another string with a single quote ' and a backslash\\\\\"\n\"Unterminated string\n'Another unterminated string'"
    );
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, RawStrings) {
    std::istringstream input("r\"This is a raw string with literal \\n and \\t\"\nR\"Another raw string with a single quote ' and two backslashs\\\\\"\nr\"Unterminated raw string\n");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, FormatStrings) {
    std::istringstream input(
        "f\"This is a format string with an expression $(1 + 2)\"\nf\"Another format string with an expression $(player.name) and a nested expression "
        "$(world.print(f\"Hello, $(player.name)!\", @a))\"\nf\"Unterminated format string"
    );
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::INTERPOLATION_START);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NUMBER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::PLUS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NUMBER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::INTERPOLATION_START);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::INTERPOLATION_START);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::INTERPOLATION_START);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::COMMA);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SELECTOR_PREFIX);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, Characters) {
    std::istringstream input("'a' '\\n' '\\t' '\\'' '\\\\' 'ab' ''");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::CHAR);
    for (int i = 0; i < 4; i++) {
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::CHAR);
    }
    for (int i = 0; i < 2; i++) {
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    }
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, CodeExample) {
    std::istringstream input("func onLoad() {\n    world.print(\"Hello, Vanillang!\", @a)\n}");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::FUNC);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_BRACE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::DOT);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::IDENTIFIER);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEFT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::STRING);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::COMMA);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::SELECTOR_PREFIX);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_PARENTHESIS);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::RIGHT_BRACE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}

TEST(VnlcLexerTest, InvalidTokens) {
    std::istringstream input("@ @1 0xG 0b102 0o8 1.2.3 \"Unterminated string\n'Unterminated char");
    VnlcLexer lexer(input);

    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    for (int i = 0; i < 6; i++) {
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::BLANK);
        ASSERT_TRUE(lexer.hasNext());
        ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    }
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::NEWLINE);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::LEXICAL_ERROR);
    ASSERT_TRUE(lexer.hasNext());
    ASSERT_EQ(lexer.next().getType(), VnlcTokenType::END_OF_FILE);
    ASSERT_FALSE(lexer.hasNext());
}