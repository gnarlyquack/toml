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
    "COMMA",
    "DAY",
    "DECIMAL",
    "EXPONENT",
    "FALSE",
    "FRACTION",
    "INFINITY",
    "KEY",
    "HEXADECIMAL",
    "HOUR",
    "LBRACE",
    "LBRACKET",
    "MINUS",
    "MINUTE",
    "MONTH",
    "NAN",
    "NEWLINE",
    "OCTAL",
    "PLUS",
    "RBRACE",
    "RBRACKET",
    "SECOND",
    "STRING",
    "TRUE",
    "YEAR",
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
        "site.\"google.com\" = true\n"
        "fruit.name = \"banana\"     # this is best practice\n"
        "fruit. color = \"yellow\"    # same as fruit.color\n"
        "fruit . flavor = \"banana\"   # same as fruit.flavor\n"
        "3.14159 = \"pi\"\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "name", 1, 1 }, { TOKEN_STRING, "Orange", 1, 8 },
        { TOKEN_KEY, "physical", 2, 1 }, { TOKEN_KEY, "color", 2, 10 }, { TOKEN_STRING, "orange", 2, 18 },
        { TOKEN_KEY, "physical", 3, 1 }, { TOKEN_KEY, "shape", 3, 10 }, { TOKEN_STRING, "round", 3, 18 },
        { TOKEN_KEY, "site", 4, 1 }, { TOKEN_KEY, "google.com", 4, 6 }, { TOKEN_TRUE, "", 4, 21 },
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
        "fruit.apple.smooth = true\n"
        "\n"
        "# So then you can add to the table \"fruit\" like so:\n"
        "fruit.orange = 2\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "fruit", 2, 1 }, { TOKEN_KEY, "apple", 2, 7 }, { TOKEN_KEY, "smooth", 2, 13 }, { TOKEN_TRUE, "", 2, 22 },
        { TOKEN_KEY, "fruit", 5, 1 }, { TOKEN_KEY, "orange", 5, 7 }, { TOKEN_DECIMAL, "2", 5, 16 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, key_redefinition)
{
    const string toml =
        "# THE FOLLOWING IS INVALID\n"
        "\n"
        "# This defines the value of fruit.apple to be an integer.\n"
        "fruit.apple = 1\n"
        "\n"
        "# But then this treats fruit.apple like it's a table.\n"
        "# You can't turn an integer into a table.\n"
        "fruit.apple.smooth = true\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "fruit", 4, 1 }, { TOKEN_KEY, "apple", 4, 7 }, { TOKEN_DECIMAL, "1", 4, 15 },
        { TOKEN_KEY, "fruit", 8, 1 }, { TOKEN_KEY, "apple", 8, 7 }, { TOKEN_KEY, "smooth", 8, 13 }, { TOKEN_TRUE, "", 8, 22 },
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


TEST(lex, floats)
{
    const string toml =
        "# fractional\n"
        "flt1 = +1.0\n"
        "flt2 = 3.1415\n"
        "flt3 = -0.01\n"
        "\n"
        "# exponent\n"
        "flt4 = 5e+22\n"
        "flt5 = 1e06\n"
        "flt6 = -2E-2\n"
        "\n"
        "# both\n"
        "flt7 = 6.626e-34\n"
        "\n"
        "flt8 = 224_617.445_991_228\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "flt1", 2, 1 }, { TOKEN_PLUS, "+", 2, 8 }, { TOKEN_DECIMAL, "1", 2, 9 }, { TOKEN_FRACTION, "0", 2, 11},
        { TOKEN_KEY, "flt2", 3, 1 }, { TOKEN_DECIMAL, "3", 3, 8 }, { TOKEN_FRACTION, "1415", 3, 10 },
        { TOKEN_KEY, "flt3", 4, 1 }, { TOKEN_MINUS, "-", 4, 8 }, { TOKEN_DECIMAL, "0", 4, 9 }, { TOKEN_FRACTION, "01", 4, 11 },
        { TOKEN_KEY, "flt4", 7, 1 }, { TOKEN_DECIMAL, "5", 7, 8 }, { TOKEN_PLUS, "+", 7, 10 }, { TOKEN_EXPONENT, "22", 7, 11 },
        { TOKEN_KEY, "flt5", 8, 1 }, { TOKEN_DECIMAL, "1", 8, 8 }, { TOKEN_EXPONENT, "06", 8, 10 },
        { TOKEN_KEY, "flt6", 9, 1 }, { TOKEN_MINUS, "-", 9, 8}, { TOKEN_DECIMAL, "2", 9, 9 }, { TOKEN_MINUS, "-", 9, 11}, { TOKEN_EXPONENT, "2", 9, 12 },
        { TOKEN_KEY, "flt7", 12, 1 }, { TOKEN_DECIMAL, "6", 12, 8 }, { TOKEN_FRACTION, "626", 12, 10 }, { TOKEN_MINUS, "-", 12, 14 }, { TOKEN_EXPONENT, "34", 12, 15 },
        { TOKEN_KEY, "flt8", 14, 1 }, { TOKEN_DECIMAL, "224617", 14, 8 }, { TOKEN_FRACTION, "445991228", 14, 16 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, invalid_floats)
{
    const string toml =
        "# INVALID FLOATS\n"
        "invalid_float_1 = .7\n"
        "invalid_float_2 = 7.\n"
        "invalid_float_3 = 3.e+20\n"
        ;

    const vector<Error> errors = {
        { 2, 19, "Missing whole part of decimal number" },
        { 3, 21, "Missing fractional part of decimal number" },
        { 4, 21, "Missing fractional part of decimal number" },
    };

    assert_errors(toml, errors);
}


TEST(lex, infinity)
{
    const string toml =
        "# infinity\n"
        "sf1 = inf  # positive infinity\n"
        "sf2 = +inf # positive infinity\n"
        "sf3 = -inf # negative infinity\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "sf1", 2, 1 }, { TOKEN_INF, "", 2, 7 },
        { TOKEN_KEY, "sf2", 3, 1 }, { TOKEN_PLUS, "+", 3, 7 }, { TOKEN_INF, "", 3, 8 },
        { TOKEN_KEY, "sf3", 4, 1 }, { TOKEN_MINUS, "-", 4, 7 }, { TOKEN_INF, "", 4, 8 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, nan)
{
    const string toml =
        "# not a number\n"
        "sf4 = nan  # actual sNaN/qNaN encoding is implementation-specific\n"
        "sf5 = +nan # same as `nan`\n"
        "sf6 = -nan # valid, actual encoding is implementation-specific\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "sf4", 2, 1 }, { TOKEN_NAN, "", 2, 7 },
        { TOKEN_KEY, "sf5", 3, 1 }, { TOKEN_PLUS, "+", 3, 7 }, { TOKEN_NAN, "", 3, 8 },
        { TOKEN_KEY, "sf6", 4, 1 }, { TOKEN_MINUS, "-", 4, 7 }, { TOKEN_NAN, "", 4, 8 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, booleans)
{
    const string toml =
        "bool1 = true\n"
        "bool2 = false\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "bool1", 1, 1}, { TOKEN_TRUE, "", 1, 9 },
        { TOKEN_KEY, "bool2", 2, 1}, { TOKEN_FALSE, "", 2, 9 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, local_times)
{
    const string toml =
        "lt1 = 07:32:00\n"
        "lt2 = 00:32:00.999999\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "lt1", 1, 1 }, { TOKEN_HOUR, "07", 1, 7 }, { TOKEN_MINUTE, "32", 1, 10 }, { TOKEN_SECOND, "00", 1, 13 },
        { TOKEN_KEY, "lt2", 2, 1 }, { TOKEN_HOUR, "00", 2, 7 }, { TOKEN_MINUTE, "32", 2, 10 }, { TOKEN_SECOND, "00", 2, 13 }, { TOKEN_FRACTION, "999999", 2, 16 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, local_dates)
{
    const string toml = "ld1 = 1979-05-27";

    const vector<Token> tokens = {
        { TOKEN_KEY, "ld1", 1, 1 }, { TOKEN_YEAR, "1979", 1, 7 }, { TOKEN_MONTH, "05", 1, 12 }, { TOKEN_DAY, "27", 1, 15 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, local_datetimes)
{
    const string toml =
        "ldt1 = 1979-05-27T07:32:00\n"
        "ldt2 = 1979-05-27T00:32:00.999999\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "ldt1", 1, 1 }, { TOKEN_YEAR, "1979", 1, 8}, { TOKEN_MONTH, "05", 1, 13 }, { TOKEN_DAY, "27", 1, 16 }, { TOKEN_HOUR, "07", 1, 19}, { TOKEN_MINUTE, "32", 1, 22 }, { TOKEN_SECOND, "00", 1, 25 },
        { TOKEN_KEY, "ldt2", 2, 1 }, { TOKEN_YEAR, "1979", 2, 8}, { TOKEN_MONTH, "05", 2, 13 }, { TOKEN_DAY, "27", 2, 16 }, { TOKEN_HOUR, "00", 2, 19}, { TOKEN_MINUTE, "32", 2, 22 }, { TOKEN_SECOND, "00", 2, 25 }, { TOKEN_FRACTION, "999999", 2, 28 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, offset_datetimes)
{
    const string toml =
        "odt1 = 1979-05-27T07:32:00Z\n"
        "odt2 = 1979-05-27T00:32:00-07:00\n"
        "odt3 = 1979-05-27T00:32:00.999999-07:00\n"
        "odt4 = 1979-05-27 07:32:00Z\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "odt1", 1, 1 }, { TOKEN_YEAR, "1979", 1, 8}, { TOKEN_MONTH, "05", 1, 13 }, { TOKEN_DAY, "27", 1, 16 }, { TOKEN_HOUR, "07", 1, 19}, { TOKEN_MINUTE, "32", 1, 22 }, { TOKEN_SECOND, "00", 1, 25 }, { TOKEN_PLUS, "+", 1, 27 }, { TOKEN_HOUR, "00", 1, 28 }, { TOKEN_MINUTE, "00", 1, 28 },
        { TOKEN_KEY, "odt2", 2, 1 }, { TOKEN_YEAR, "1979", 2, 8}, { TOKEN_MONTH, "05", 2, 13 }, { TOKEN_DAY, "27", 2, 16 }, { TOKEN_HOUR, "00", 2, 19}, { TOKEN_MINUTE, "32", 2, 22 }, { TOKEN_SECOND, "00", 2, 25 }, { TOKEN_MINUS, "-", 2, 27 }, { TOKEN_HOUR, "07", 2, 28 }, { TOKEN_MINUTE, "00", 2, 31 },
        { TOKEN_KEY, "odt3", 3, 1 }, { TOKEN_YEAR, "1979", 3, 8}, { TOKEN_MONTH, "05", 3, 13 }, { TOKEN_DAY, "27", 3, 16 }, { TOKEN_HOUR, "00", 3, 19}, { TOKEN_MINUTE, "32", 3, 22 }, { TOKEN_SECOND, "00", 3, 25 }, { TOKEN_FRACTION, "999999", 3, 28 }, { TOKEN_MINUS, "-", 3, 34 }, { TOKEN_HOUR, "07", 3, 35 }, { TOKEN_MINUTE, "00", 3, 38 },
        { TOKEN_KEY, "odt4", 4, 1 }, { TOKEN_YEAR, "1979", 4, 8}, { TOKEN_MONTH, "05", 4, 13 }, { TOKEN_DAY, "27", 4, 16 }, { TOKEN_HOUR, "07", 4, 19}, { TOKEN_MINUTE, "32", 4, 22 }, { TOKEN_SECOND, "00", 4, 25 }, { TOKEN_PLUS, "+", 4, 27 }, { TOKEN_HOUR, "00", 4, 28 }, { TOKEN_MINUTE, "00", 4, 28 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, arrays)
{
    const string toml =
        "integers = [ 1, 2, 3 ]\n"
        "colors = [ \"red\", \"yellow\", \"green\" ]\n"
        "nested_arrays_of_ints = [ [ 1, 2 ], [3, 4, 5] ]\n"
        "nested_mixed_array = [ [ 1, 2 ], [\"a\", \"b\", \"c\"] ]\n"
        "string_array = [ \"all\", 'strings', \"\"\"are the same\"\"\", '''type''' ]\n"
        "\n"
        "# Mixed-type arrays are allowed\n"
        "numbers = [ 0.1, 0.2, 0.5, 1, 2, 5 ]\n"
        "contributors = [\n"
        "  \"Foo Bar <foo@example.com>\",\n"
        "  { name = \"Baz Qux\", email = \"bazqux@example.com\", url = \"https://example.com/bazqux\" }\n"
        "]\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "integers", 1, 1 }, { TOKEN_LBRACKET, "", 1, 12 },
            { TOKEN_DECIMAL, "1", 1, 14 }, { TOKEN_COMMA, "", 1, 15 },
            { TOKEN_DECIMAL, "2", 1, 17 }, { TOKEN_COMMA, "", 1, 18 },
            { TOKEN_DECIMAL, "3", 1, 20 }, { TOKEN_RBRACKET, "", 1, 22 },
        { TOKEN_KEY, "colors", 2, 1 }, { TOKEN_LBRACKET, "", 2, 10 },
            { TOKEN_STRING, "red", 2, 12 }, { TOKEN_COMMA, "", 2, 17 },
            { TOKEN_STRING, "yellow", 2, 19 }, { TOKEN_COMMA, "", 2, 27 },
            { TOKEN_STRING, "green", 2, 29 }, { TOKEN_RBRACKET, "", 2, 37 },
        { TOKEN_KEY, "nested_arrays_of_ints", 3, 1 }, { TOKEN_LBRACKET, "", 3, 25 },
            { TOKEN_LBRACKET, "", 3, 27 },
                { TOKEN_DECIMAL, "1", 3, 29 }, { TOKEN_COMMA, "", 3, 30 },
                { TOKEN_DECIMAL, "2", 3, 32 },
            { TOKEN_RBRACKET, "", 3, 34 }, { TOKEN_COMMA, "", 3, 35 },
            { TOKEN_LBRACKET, "", 3, 37 },
                { TOKEN_DECIMAL, "3", 3, 38 }, { TOKEN_COMMA, "", 3, 39 },
                { TOKEN_DECIMAL, "4", 3, 41 }, { TOKEN_COMMA, "", 3, 42 },
                { TOKEN_DECIMAL, "5", 3, 44 },
            { TOKEN_RBRACKET, "", 3, 45 },
        { TOKEN_RBRACKET, "", 3, 47 },
        { TOKEN_KEY, "nested_mixed_array", 4, 1 }, { TOKEN_LBRACKET, "", 4, 22 },
            { TOKEN_LBRACKET, "", 4, 24 },
                { TOKEN_DECIMAL, "1", 4, 26 }, { TOKEN_COMMA, "", 4, 27 },
                { TOKEN_DECIMAL, "2", 4, 29 },
            { TOKEN_RBRACKET, "", 4, 31 }, { TOKEN_COMMA, "", 4, 32 },
            { TOKEN_LBRACKET, "", 4, 34 },
                { TOKEN_STRING, "a", 4, 35 }, { TOKEN_COMMA, "", 4, 38 },
                { TOKEN_STRING, "b", 4, 40 }, { TOKEN_COMMA, "", 4, 43 },
                { TOKEN_STRING, "c", 4, 45 },
            { TOKEN_RBRACKET, "", 4, 48 },
        { TOKEN_RBRACKET, "", 4, 50 },
        { TOKEN_KEY, "string_array", 5, 1 }, { TOKEN_LBRACKET, "", 5, 16 },
            { TOKEN_STRING, "all", 5, 18 }, { TOKEN_COMMA, "", 5, 23 },
            { TOKEN_STRING, "strings", 5, 25 }, { TOKEN_COMMA, "", 5, 34 },
            { TOKEN_STRING, "are the same", 5, 36 }, { TOKEN_COMMA, "", 5, 54 },
            { TOKEN_STRING, "type", 5, 56 }, { TOKEN_RBRACKET, "", 5, 67 },
        { TOKEN_KEY, "numbers", 8, 1 }, { TOKEN_LBRACKET, "", 8, 11 },
            { TOKEN_DECIMAL, "0", 8, 13 }, { TOKEN_FRACTION, "1", 8, 15 }, { TOKEN_COMMA, "", 8, 16 },
            { TOKEN_DECIMAL, "0", 8, 18 }, { TOKEN_FRACTION, "2", 8, 20 }, { TOKEN_COMMA, "", 8, 21 },
            { TOKEN_DECIMAL, "0", 8, 23 }, { TOKEN_FRACTION, "5", 8, 25 }, { TOKEN_COMMA, "", 8, 26 },
            { TOKEN_DECIMAL, "1", 8, 28 }, { TOKEN_COMMA, "", 8, 29 },
            { TOKEN_DECIMAL, "2", 8, 31 }, { TOKEN_COMMA, "", 8, 32 },
            { TOKEN_DECIMAL, "5", 8, 34 }, { TOKEN_RBRACKET, "", 8, 36 },
        { TOKEN_KEY, "contributors", 9, 1 }, { TOKEN_LBRACKET, "", 9, 16 },
            { TOKEN_STRING, "Foo Bar <foo@example.com>", 10, 3 }, { TOKEN_COMMA, "", 10, 30 },
            { TOKEN_LBRACE, "", 11, 3 },
                { TOKEN_KEY, "name", 11, 5 }, { TOKEN_STRING, "Baz Qux", 11, 12 }, { TOKEN_COMMA, "", 11, 21},
                { TOKEN_KEY, "email", 11, 23 }, { TOKEN_STRING, "bazqux@example.com", 11, 31 }, { TOKEN_COMMA, "", 11, 51},
                { TOKEN_KEY, "url", 11, 53 }, { TOKEN_STRING, "https://example.com/bazqux", 11, 59 }, { TOKEN_RBRACE, "", 11, 88},
            { TOKEN_RBRACKET, "", 12, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, inline_tables)
{
    const string toml =
        "name = { first = \"Tom\", last = \"Preston-Werner\" }\n"
        "point = { x = 1, y = 2 }\n"
        "animal = { type.name = \"pug\" }\n"
        ;

    const vector<Token> tokens = {
        { TOKEN_KEY, "name", 1, 1 }, { TOKEN_LBRACE, "", 1, 8},
            { TOKEN_KEY, "first", 1, 10 }, { TOKEN_STRING, "Tom", 1, 18 }, { TOKEN_COMMA, "", 1, 23 },
            { TOKEN_KEY, "last", 1, 25 }, { TOKEN_STRING, "Preston-Werner", 1, 32 }, { TOKEN_RBRACE, "", 1, 49 },
        { TOKEN_KEY, "point", 2, 1 }, { TOKEN_LBRACE, "", 2, 9},
            { TOKEN_KEY, "x", 2, 11 }, { TOKEN_DECIMAL, "1", 2, 15 }, { TOKEN_COMMA, "", 2, 16 },
            { TOKEN_KEY, "y", 2, 18 }, { TOKEN_DECIMAL, "2", 2, 22 }, { TOKEN_RBRACE, "", 2, 24 },
        { TOKEN_KEY, "animal", 3, 1 }, { TOKEN_LBRACE, "", 3, 10},
            { TOKEN_KEY, "type", 3, 12 }, { TOKEN_KEY, "name", 3, 17 }, { TOKEN_STRING, "pug", 3, 24 },
            { TOKEN_RBRACE, "", 3, 30 },
    };

    assert_lexed(toml, tokens);
}
