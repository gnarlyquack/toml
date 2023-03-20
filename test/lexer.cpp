//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include <gtest/gtest.h>

#include "lexer.hpp"


using namespace std;
using namespace toml;


namespace toml
{


constexpr const char *TOKEN_NAMES[] = {
    "ERROR",
    "BINARY",
    "DECIMAL",
    "KEY",
    "HEXADECIMAL",
    "MINUS",
    "OCTAL",
    "PLUS",
    "STRING",
};


inline bool
operator==(const Token &left, const Token &right)
{
    bool result =
        (left.type == right.type)
        && (left.line == right.line)
        && (left.column == right.column)
        && (left.lexeme == right.lexeme);
    return result;
}


inline std::ostream& operator<<(std::ostream &os, const Token &token)
{
    os << "Token("
        << TOKEN_NAMES[token.type] << ", \""
        << token.lexeme << "\", "
        << token.line << ", "
        << token.column  << ")";
    return os;
}


inline bool
operator==(const Error &left, const Error &right)
{
    bool result =
        (left.line == right.line)
        && (left.column == right.column)
        && (left.message == right.message);
    return result;
}


inline std::ostream& operator<<(std::ostream &os, const Error &error)
{
    os << "Error(" << error.line << ", " << error.column << ", \"" << error.message << "\")";
    return os;
}


} // namespace toml


namespace
{


void
assert_lexed(const string &toml, const vector<Token> &expected)
{
    vector<Token> actual;
    vector<Error> errors;
    bool result = lex_toml(toml, actual, errors);

    EXPECT_TRUE(result);
    EXPECT_EQ(actual, expected);
    EXPECT_EQ(errors, vector<Error>{});
}


void
assert_errors(const string &toml, const vector<Error> &expected)
{
    vector<Token> tokens;
    vector<Error> actual;
    bool result = lex_toml(toml, tokens, actual);

    ASSERT_FALSE(result);
    ASSERT_EQ(actual, expected);
}


} // namespace



TEST(lex, comments)
{
    const string toml =
        "# This is a full-line comment\n"
        "key = \"value\"  # This is a comment at the end of a line\n"
        "another = \"# This is not a comment\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "key", 2, 1 }, { TOKEN_STRING, "value", 2, 7 },
        { TOKEN_KEY, "another", 3, 1 }, { TOKEN_STRING, "# This is not a comment", 3, 11 } ,
    };

    assert_lexed(toml, tokens);
}


TEST(lex, keyvals)
{
    const string toml = "key = \"value\"";

    const vector<Token> tokens = {
        { TOKEN_KEY, "key", 1, 1 }, { TOKEN_STRING, "value", 1, 7 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, invalid_keyvals)
{
    const string toml =
        "key = # INVALID\n"
        "first = \"Tom\" last = \"Preston-Werner\" # INVALID\n"
        ;

    const vector<Error> errors = {
        { 1, 7, "Missing value" },
        { 2, 15, "Expected new line but got: last = \"Preston-Werner\" # INVALID" },
    };

    assert_errors(toml, errors);
}


TEST(lex, bare_keys)
{
    const string toml =
        "key = \"value\"\n"
        "bare_key = \"value\"\n"
        "bare-key = \"value\"\n"
        "1234 = \"value\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "key", 1, 1 }, { TOKEN_STRING, "value", 1, 7 },
        { TOKEN_KEY, "bare_key", 2, 1 }, { TOKEN_STRING, "value", 2, 12 },
        { TOKEN_KEY, "bare-key", 3, 1 }, { TOKEN_STRING, "value", 3, 12 },
        { TOKEN_KEY, "1234", 4, 1 }, { TOKEN_STRING, "value", 4, 8 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, quoted_keys)
{
    const string toml =
        "\"127.0.0.1\" = \"value\"\n"
        "\"character encoding\" = \"value\"\n"
        "\"ʎǝʞ\" = \"value\"\n"
        "'key2' = \"value\"\n"
        "'quoted \"value\"' = \"value\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "127.0.0.1", 1, 1 }, { TOKEN_STRING, "value", 1, 15 },
        { TOKEN_KEY, "character encoding", 2, 1 }, { TOKEN_STRING, "value", 2, 24 },
        { TOKEN_KEY, "ʎǝʞ", 3, 1 }, { TOKEN_STRING, "value", 3, 9 },
        { TOKEN_KEY, "key2", 4, 1 }, { TOKEN_STRING, "value", 4, 10 },
        { TOKEN_KEY, "quoted \"value\"", 5, 1 }, { TOKEN_STRING, "value", 5, 20 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, empty_unqoted_key)
{
    const string toml = "= \"no key name\"  # INVALID";

    const vector<Error> errors = {
        { 1, 1, "Missing key" },
    };

    assert_errors(toml, errors);
}


TEST(lex, empty_string_key)
{
    const string toml = "\"\" = \"blank\"     # VALID but discouraged";

    const vector<Token> tokens = {
        { TOKEN_KEY, "", 1, 1 }, { TOKEN_STRING, "blank", 1, 6 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, empty_literal_key)
{
    const string toml = "'' = 'blank'     # VALID but discouraged";

    const vector<Token> tokens = {
        { TOKEN_KEY, "", 1, 1 }, { TOKEN_STRING, "blank", 1, 6 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, dotted_keys)
{
    const string toml =
        "name = \"Orange\"\n"
        "physical.color = \"orange\"\n"
        "physical.shape = \"round\"\n"
        "site.\"google.com\" = \"true\"\n"
        "fruit.name = \"banana\"     # this is best practice\n"
        "fruit. color = \"yellow\"    # same as fruit.color\n"
        "fruit . flavor = \"banana\"   # same as fruit.flavor\n"
        "3.14159 = \"pi\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "name", 1, 1 }, { TOKEN_STRING, "Orange", 1, 8 },
        { TOKEN_KEY, "physical", 2, 1 }, { TOKEN_KEY, "color", 2, 10 }, { TOKEN_STRING, "orange", 2, 18 },
        { TOKEN_KEY, "physical", 3, 1 }, { TOKEN_KEY, "shape", 3, 10 }, { TOKEN_STRING, "round", 3, 18 },
        { TOKEN_KEY, "site", 4, 1 }, { TOKEN_KEY, "google.com", 4, 6 }, { TOKEN_STRING, "true", 4, 21 },
        { TOKEN_KEY, "fruit", 5, 1 }, { TOKEN_KEY, "name", 5, 7 }, { TOKEN_STRING, "banana", 5, 14 },
        { TOKEN_KEY, "fruit", 6, 1 }, { TOKEN_KEY, "color", 6, 8 }, { TOKEN_STRING, "yellow", 6, 16 },
        { TOKEN_KEY, "fruit", 7, 1 }, { TOKEN_KEY, "flavor", 7, 9 }, { TOKEN_STRING, "banana", 7, 18 },
        { TOKEN_KEY, "3", 8, 1 }, { TOKEN_KEY, "14159", 8, 3 }, { TOKEN_STRING, "pi", 8, 11 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, multiple_keys)
{
    const string toml =
        "# DO NOT DO THIS\n"
        "name = \"Tom\"\n"
        "name = \"Pradyun\"\n"
        "# THIS WILL NOT WORK\n"
        "spelling = \"favorite\"\n"
        "\"spelling\" = \"favourite\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "name", 2, 1 }, { TOKEN_STRING, "Tom", 2, 8 },
        { TOKEN_KEY, "name", 3, 1 }, { TOKEN_STRING, "Pradyun", 3, 8 },
        { TOKEN_KEY, "spelling", 5, 1 }, { TOKEN_STRING, "favorite", 5, 12 },
        { TOKEN_KEY, "spelling", 6, 1 }, { TOKEN_STRING, "favourite", 6, 14 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, extend_implicit_key)
{
    const string toml =
        "# This makes the key \"fruit\" into a table.\n"
        "fruit.apple.smooth = \"true\"\n"
        "\n"
        "# So then you can add to the table \"fruit\" like so:\n"
        "fruit.orange = \"2\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "fruit", 2, 1 }, { TOKEN_KEY, "apple", 2, 7 }, { TOKEN_KEY, "smooth", 2, 13 }, { TOKEN_STRING, "true", 2, 22 },
        { TOKEN_KEY, "fruit", 5, 1 }, { TOKEN_KEY, "orange", 5, 7 }, { TOKEN_STRING, "2", 5, 16 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, key_redefinition)
{
    const string toml =
        "# THE FOLLOWING IS INVALID\n"
        "\n"
        "# This defines the value of fruit.apple to be an integer.\n"
        "fruit.apple = \"1\"\n"
        "\n"
        "# But then this treats fruit.apple like it's a table.\n"
        "# You can't turn an integer into a table.\n"
        "fruit.apple.smooth = \"true\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "fruit", 4, 1 }, { TOKEN_KEY, "apple", 4, 7 }, { TOKEN_STRING, "1", 4, 15 },
        { TOKEN_KEY, "fruit", 8, 1 }, { TOKEN_KEY, "apple", 8, 7 }, { TOKEN_KEY, "smooth", 8, 13 }, { TOKEN_STRING, "true", 8, 22 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, out_of_order_keys)
{
    const string toml =
        "# VALID BUT DISCOURAGED\n"
        "\n"
        "apple.type = \"fruit\"\n"
        "orange.type = \"fruit\"\n"
        "\n"
        "apple.skin = \"thin\"\n"
        "orange.skin = \"thick\"\n"
        "\n"
        "apple.color = \"red\"\n"
        "orange.color = \"orange\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "apple", 3, 1 }, { TOKEN_KEY, "type", 3, 7 }, { TOKEN_STRING, "fruit", 3, 14 },
        { TOKEN_KEY, "orange", 4, 1 }, { TOKEN_KEY, "type", 4, 8 }, { TOKEN_STRING, "fruit", 4, 15 },
        { TOKEN_KEY, "apple", 6, 1 }, { TOKEN_KEY, "skin", 6, 7 }, { TOKEN_STRING, "thin", 6, 14 },
        { TOKEN_KEY, "orange", 7, 1 }, { TOKEN_KEY, "skin", 7, 8 }, { TOKEN_STRING, "thick", 7, 15 },
        { TOKEN_KEY, "apple", 9, 1 }, { TOKEN_KEY, "color", 9, 7 }, { TOKEN_STRING, "red", 9, 15 },
        { TOKEN_KEY, "orange", 10, 1 }, { TOKEN_KEY, "color", 10, 8 }, { TOKEN_STRING, "orange", 10, 16 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, basic_strings)
{
    const string toml =
        "str = \"I'm a string. \\\"You can quote me\\\". Name\\tJos\\u00e9\\nLocation\\tSF.\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "str", 1, 1 }, { TOKEN_STRING, "I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF.", 1, 7 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, multiline_basic_strings)
{
    const string toml =
        "str1 = \"\"\"\n"
        "Roses are red\n"
        "Violets are blue\"\"\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "str1", 1, 1 }, { TOKEN_STRING, "Roses are red\nViolets are blue", 1, 8 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, line_ending_backslash)
{
    const string toml =
        "# The following strings are byte-for-byte equivalent:\n"
        "str1 = \"The quick brown fox jumps over the lazy dog.\"\n"
        "\n"
        "str2 = \"\"\"\n"
        "The quick brown \\\n"
        "\n"
        "\n"
        "  fox jumps over \\\n"
        "    the lazy dog.\"\"\"\n"
        "\n"
        "str3 = \"\"\"\\\n"
        "       The quick brown \\\n"
        "       fox jumps over \\\n"
        "       the lazy dog.\\\n"
        "       \"\"\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "str1", 2, 1 }, { TOKEN_STRING, "The quick brown fox jumps over the lazy dog.", 2, 8 },
        { TOKEN_KEY, "str2", 4, 1 }, { TOKEN_STRING, "The quick brown fox jumps over the lazy dog.", 4, 8 },
        { TOKEN_KEY, "str3", 11, 1 }, { TOKEN_STRING, "The quick brown fox jumps over the lazy dog.", 11, 8 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, multiline_basic_string_escapes)
{
    const string toml =
        "str4 = \"\"\"Here are two quotation marks: \"\". Simple enough.\"\"\"\n"
        "# str5 = \"\"\"Here are three quotation marks: \"\"\".\"\"\"  # INVALID\n"
        "str5 = \"\"\"Here are three quotation marks: \"\"\\\".\"\"\"\n"
        "str6 = \"\"\"Here are fifteen quotation marks: \"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\".\"\"\"\n"
        "\n"
        "# \"This,\" she said, \"is just a pointless statement.\"\n"
        "str7 = \"\"\"\"This,\" she said, \"is just a pointless statement.\"\"\"\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "str4", 1, 1 }, { TOKEN_STRING, "Here are two quotation marks: \"\". Simple enough.", 1, 8 },
        { TOKEN_KEY, "str5", 3, 1 }, { TOKEN_STRING, "Here are three quotation marks: \"\"\".", 3, 8 },
        { TOKEN_KEY, "str6", 4, 1 }, { TOKEN_STRING, "Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\".", 4, 8 },
        { TOKEN_KEY, "str7", 7, 1 }, { TOKEN_STRING, "\"This,\" she said, \"is just a pointless statement.\"", 7, 8 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, literal_strings)
{
    const string toml =
        "# What you see is what you get.\n"
        "winpath  = 'C:\\Users\\nodejs\\templates'\n"
        "winpath2 = '\\\\ServerX\\admin$\\system32\\'\n"
        "quoted   = 'Tom \"Dubs\" Preston-Werner'\n"
        "regex    = '<\\i\\c*\\s*>'\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "winpath", 2, 1 }, { TOKEN_STRING, "C:\\Users\\nodejs\\templates", 2, 12 },
        { TOKEN_KEY, "winpath2", 3, 1 }, { TOKEN_STRING, "\\\\ServerX\\admin$\\system32\\", 3, 12 },
        { TOKEN_KEY, "quoted", 4, 1 }, { TOKEN_STRING, "Tom \"Dubs\" Preston-Werner", 4, 12 },
        { TOKEN_KEY, "regex", 5, 1 }, { TOKEN_STRING, "<\\i\\c*\\s*>", 5, 12 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, multiline_literal_strings)
{
    const string toml =
        "regex2 = '''I [dw]on't need \\d{2} apples'''\n"
        "lines  = '''\n"
        "The first newline is\n"
        "trimmed in raw strings.\n"
        "   All other whitespace\n"
        "   is preserved.\n"
        "'''\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "regex2", 1, 1 }, { TOKEN_STRING, "I [dw]on't need \\d{2} apples", 1, 10 },
        { TOKEN_KEY, "lines", 2, 1 }, { TOKEN_STRING, "The first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n", 2, 10 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, mulitiline_literal_string_escapes)
{
    const string toml =
        "quot15 = '''Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"'''\n"
        "\n"
        "# apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID\n"
        "apos15 = \"Here are fifteen apostrophes: '''''''''''''''\"\n"
        "\n"
        "# 'That,' she said, 'is still pointless.'\n"
        "str = ''''That,' she said, 'is still pointless.''''\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "quot15", 1, 1 }, { TOKEN_STRING, "Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"", 1, 10 },
        { TOKEN_KEY, "apos15", 4, 1 }, { TOKEN_STRING, "Here are fifteen apostrophes: '''''''''''''''", 4, 10 },
        { TOKEN_KEY, "str", 7, 1}, { TOKEN_STRING, "'That,' she said, 'is still pointless.'", 7, 7 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, integers)
{
    const string toml =
        "int1 = +99\n"
        "int2 = 42\n"
        "int3 = 0\n"
        "int4 = -17\n"
        "int5 = 1_000\n"
        "int6 = 5_349_221\n"
        "int7 = 53_49_221  # Indian number system grouping\n"
        "int8 = 1_2_3_4_5  # VALID but discouraged\n"
        "\n"
        "# hexadecimal with prefix `0x`\n"
        "hex1 = 0xDEADBEEF\n"
        "hex2 = 0xdeadbeef\n"
        "hex3 = 0xdead_beef\n"
        "\n"
        "# octal with prefix `0o`\n"
        "oct1 = 0o01234567\n"
        "oct2 = 0o755 # useful for Unix file permissions\n"
        "\n"
        "# binary with prefix `0b`\n"
        "bin1 = 0b11010110\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "int1", 1, 1 }, { TOKEN_PLUS, "+", 1, 8 }, { TOKEN_DECIMAL, "99", 1, 9 },
        { TOKEN_KEY, "int2", 2, 1 }, { TOKEN_DECIMAL, "42", 2, 8 },
        { TOKEN_KEY, "int3", 3, 1 }, { TOKEN_DECIMAL, "0", 3, 8 },
        { TOKEN_KEY, "int4", 4, 1 }, { TOKEN_MINUS, "-", 4, 8 }, { TOKEN_DECIMAL, "17", 4, 9 },
        { TOKEN_KEY, "int5", 5, 1 }, { TOKEN_DECIMAL, "1000", 5, 8 },
        { TOKEN_KEY, "int6", 6, 1 }, { TOKEN_DECIMAL, "5349221", 6, 8 },
        { TOKEN_KEY, "int7", 7, 1 }, { TOKEN_DECIMAL, "5349221", 7, 8 },
        { TOKEN_KEY, "int8", 8, 1 }, { TOKEN_DECIMAL, "12345", 8, 8 },
        { TOKEN_KEY, "hex1", 11, 1 }, { TOKEN_HEXADECIMAL, "DEADBEEF", 11, 8 },
        { TOKEN_KEY, "hex2", 12, 1 }, { TOKEN_HEXADECIMAL, "deadbeef", 12, 8 },
        { TOKEN_KEY, "hex3", 13, 1 }, { TOKEN_HEXADECIMAL, "deadbeef", 13, 8 },
        { TOKEN_KEY, "oct1", 16, 1 }, { TOKEN_OCTAL, "01234567", 16, 8 },
        { TOKEN_KEY, "oct2", 17, 1 }, { TOKEN_OCTAL, "755", 17, 8 },
        { TOKEN_KEY, "bin1", 20, 1 }, { TOKEN_BINARY, "11010110", 20, 8 },
    };

    assert_lexed(toml, tokens);
}
