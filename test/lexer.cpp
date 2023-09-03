//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include "test_common.hpp"

#include <gtest/gtest.h>


using namespace std;
using namespace toml;


namespace
{


void
assert_lexed(const string &toml, vector<Token> &expected)
{
    vector<Token> actual;
    vector<Error> errors;
    bool result = lex_toml(toml, actual, errors);

    EXPECT_TRUE(result);
    EXPECT_EQ(actual, expected);
    EXPECT_EQ(errors, vector<Error>{});

    for (Token &token : expected)
    {
        delete token.value;
    }
    for (Token &token : actual)
    {
        delete token.value;
    }
}


void
assert_lex_errors(const string &toml, const vector<Error> &expected)
{
    vector<Token> tokens;
    vector<Error> actual;
    bool result = lex_toml(toml, tokens, actual);

    ASSERT_FALSE(result);
    ASSERT_EQ(actual, expected);

    for (Token &token : tokens)
    {
        delete token.value;
    }
}


} // namespace


TEST(lex, comments)
{
    const string toml =
        "# This is a full-line comment\n"
        "key = \"value\"  # This is a comment at the end of a line\n"
        "another = \"# This is not a comment\"\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "key", 30, 2, 1 },
        { TOKEN_VALUE, new StringValue("value"), "\"value\"", 36, 2, 7 },
        { TOKEN_KEY, nullptr, "another", 86, 3, 1 },
        { TOKEN_VALUE, new StringValue("# This is not a comment"), "\"# This is not a comment\"", 96, 3, 11 } ,
        { TOKEN_EOF, nullptr, "", 122, 4, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, keyvals)
{
    const string toml = "key = \"value\"";

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "key", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 6, 1, 7 },
        { TOKEN_EOF, nullptr, "", 13, 1, 14 },
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
        { 1, 7, "Missing value." },
        { 2, 15, "Expected the end of the line but got: last = \"Preston-Werner\"" },
    };

    assert_lex_errors(toml, errors);
}


TEST(lex, bare_keys)
{
    const string toml =
        "key = \"value\"\n"
        "bare_key = \"value\"\n"
        "bare-key = \"value\"\n"
        "1234 = \"value\"\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "key", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 6, 1, 7 },
        { TOKEN_KEY, nullptr, "bare_key", 14, 2, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 25, 2, 12 },
        { TOKEN_KEY, nullptr, "bare-key", 33, 3, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 44, 3, 12 },
        { TOKEN_KEY, nullptr, "1234", 52, 4, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 59, 4, 8 },
        { TOKEN_EOF, nullptr, "", 67, 5, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "127.0.0.1", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 14, 1, 15 },
        { TOKEN_KEY, nullptr, "character encoding", 22, 2, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 45, 2, 24 },
        { TOKEN_KEY, nullptr, "ʎǝʞ", 53, 3, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 64, 3, 9 },
        { TOKEN_KEY, nullptr, "key2", 72, 4, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 81, 4, 10 },
        { TOKEN_KEY, nullptr, "quoted \"value\"", 89, 5, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 108, 5, 20 },
        { TOKEN_EOF, nullptr, "", 116, 6, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, empty_unqoted_key)
{
    const string toml = "= \"no key name\"  # INVALID";

    const vector<Error> errors = {
        { 1, 1, "Missing key." },
    };

    assert_lex_errors(toml, errors);
}


TEST(lex, empty_string_key)
{
    const string toml = "\"\" = \"blank\"     # VALID but discouraged";

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("blank"), "\"blank\"", 5, 1, 6 },
        { TOKEN_EOF, nullptr, "", 40, 1, 41 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, empty_literal_key)
{
    const string toml = "'' = 'blank'     # VALID but discouraged";

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("blank"), "'blank'", 5, 1, 6 },
        { TOKEN_EOF, nullptr, "", 40, 1, 41 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "name", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("Orange"), "\"Orange\"", 7, 1, 8 },
        { TOKEN_KEY, nullptr, "physical", 16, 2, 1 }, { TOKEN_KEY, nullptr, "color", 25, 2, 10 }, { TOKEN_VALUE, new StringValue("orange"), "\"orange\"", 33, 2, 18 },
        { TOKEN_KEY, nullptr, "physical", 42, 3, 1 }, { TOKEN_KEY, nullptr, "shape", 51, 3, 10 }, { TOKEN_VALUE, new StringValue("round"), "\"round\"", 59, 3, 18 },
        { TOKEN_KEY, nullptr, "site", 67, 4, 1 }, { TOKEN_KEY, nullptr, "google.com", 72, 4, 6 }, { TOKEN_VALUE, new BooleanValue(true), "true", 87, 4, 21 },
        { TOKEN_KEY, nullptr, "fruit", 92, 5, 1 }, { TOKEN_KEY, nullptr, "name", 98, 5, 7 }, { TOKEN_VALUE, new StringValue("banana"), "\"banana\"", 105, 5, 14 },
        { TOKEN_KEY, nullptr, "fruit", 142, 6, 1 }, { TOKEN_KEY, nullptr, "color", 149, 6, 8 }, { TOKEN_VALUE, new StringValue("yellow"), "\"yellow\"", 157, 6, 16 },
        { TOKEN_KEY, nullptr, "fruit", 191, 7, 1 }, { TOKEN_KEY, nullptr, "flavor", 199, 7, 9 }, { TOKEN_VALUE, new StringValue("banana"), "\"banana\"", 208, 7, 18 },
        { TOKEN_KEY, nullptr, "3", 242, 8, 1 }, { TOKEN_KEY, nullptr, "14159", 244, 8, 3 }, { TOKEN_VALUE, new StringValue("pi"), "\"pi\"", 252, 8, 11 },
        { TOKEN_EOF, nullptr, "", 257, 9, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "name", 17, 2, 1 }, { TOKEN_VALUE, new StringValue("Tom"), "\"Tom\"", 24, 2, 8 },
        { TOKEN_KEY, nullptr, "name", 30, 3, 1 }, { TOKEN_VALUE, new StringValue("Pradyun"), "\"Pradyun\"", 37, 3, 8 },
        { TOKEN_KEY, nullptr, "spelling", 68, 5, 1 }, { TOKEN_VALUE, new StringValue("favorite"), "\"favorite\"", 79, 5, 12 },
        { TOKEN_KEY, nullptr, "spelling", 90, 6, 1 }, { TOKEN_VALUE, new StringValue("favourite"), "\"favourite\"", 103, 6, 14 },
        { TOKEN_EOF, nullptr, "", 115, 7, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "fruit", 43, 2, 1 }, { TOKEN_KEY, nullptr, "apple", 49, 2, 7 }, { TOKEN_KEY, nullptr, "smooth", 55, 2, 13 }, { TOKEN_VALUE, new BooleanValue(true), "true", 64, 2, 22 },
        { TOKEN_KEY, nullptr, "fruit", 122, 5, 1 }, { TOKEN_KEY, nullptr, "orange", 128, 5, 7 }, { TOKEN_VALUE, new IntegerValue(2), "2", 137, 5, 16 },
        { TOKEN_EOF, nullptr, "", 139, 6, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "fruit", 86, 4, 1 }, { TOKEN_KEY, nullptr, "apple", 92, 4, 7 }, { TOKEN_VALUE, new IntegerValue(1), "1", 100, 4, 15 },
        { TOKEN_KEY, nullptr, "fruit", 199, 8, 1 }, { TOKEN_KEY, nullptr, "apple", 205, 8, 7 }, { TOKEN_KEY, nullptr, "smooth", 211, 8, 13 }, { TOKEN_VALUE, new BooleanValue(true), "true", 220, 8, 22 },
        { TOKEN_EOF, nullptr, "", 225, 9, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "apple", 25, 3, 1 }, { TOKEN_KEY, nullptr, "type", 31, 3, 7 }, { TOKEN_VALUE, new StringValue("fruit"), "\"fruit\"", 38, 3, 14 },
        { TOKEN_KEY, nullptr, "orange", 46, 4, 1 }, { TOKEN_KEY, nullptr, "type", 53, 4, 8 }, { TOKEN_VALUE, new StringValue("fruit"), "\"fruit\"", 60, 4, 15 },
        { TOKEN_KEY, nullptr, "apple", 69, 6, 1 }, { TOKEN_KEY, nullptr, "skin", 75, 6, 7 }, { TOKEN_VALUE, new StringValue("thin"), "\"thin\"", 82, 6, 14 },
        { TOKEN_KEY, nullptr, "orange", 89, 7, 1 }, { TOKEN_KEY, nullptr, "skin", 96, 7, 8 }, { TOKEN_VALUE, new StringValue("thick"), "\"thick\"", 103, 7, 15 },
        { TOKEN_KEY, nullptr, "apple", 112, 9, 1 }, { TOKEN_KEY, nullptr, "color", 118, 9, 7 }, { TOKEN_VALUE, new StringValue("red"), "\"red\"", 126, 9, 15 },
        { TOKEN_KEY, nullptr, "orange", 132, 10, 1 }, { TOKEN_KEY, nullptr, "color", 139, 10, 8 }, { TOKEN_VALUE, new StringValue("orange"), "\"orange\"", 147, 10, 16 },
        { TOKEN_EOF, nullptr, "", 156, 11, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, basic_strings)
{
    const string toml =
        "str = \"I'm a string. \\\"You can quote me\\\". Name\\tJos\\u00e9\\nLocation\\tSF.\"\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "str", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF."), "\"I'm a string. \\\"You can quote me\\\". Name\\tJos\\u00e9\\nLocation\\tSF.\"", 6, 1, 7 },
        { TOKEN_EOF, nullptr, "", 75, 2, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "str1", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("Roses are red\nViolets are blue"), "\"\"\"\nRoses are red\nViolets are blue\"\"\"", 7, 1, 8 },
        { TOKEN_EOF, nullptr, "", 45, 4, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "str1", 54, 2, 1 }, { TOKEN_VALUE, new StringValue("The quick brown fox jumps over the lazy dog."), "\"The quick brown fox jumps over the lazy dog.\"", 61, 2, 8 },
        { TOKEN_KEY, nullptr, "str2", 109, 4, 1 }, { TOKEN_VALUE, new StringValue("The quick brown fox jumps over the lazy dog."), "\"\"\"\nThe quick brown \\\n\n\n  fox jumps over \\\n    the lazy dog.\"\"\"", 116, 4, 8 },
        { TOKEN_KEY, nullptr, "str3", 181, 11, 1 }, { TOKEN_VALUE, new StringValue("The quick brown fox jumps over the lazy dog."), "\"\"\"\\\n       The quick brown \\\n       fox jumps over \\\n       the lazy dog.\\\n       \"\"\"", 188, 11, 8 },
        { TOKEN_EOF, nullptr, "", 275, 16, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "str4", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("Here are two quotation marks: \"\". Simple enough."), "\"\"\"Here are two quotation marks: \"\". Simple enough.\"\"\"", 7, 1, 8 },
        { TOKEN_KEY, nullptr, "str5", 125, 3, 1 }, { TOKEN_VALUE, new StringValue("Here are three quotation marks: \"\"\"."), "\"\"\"Here are three quotation marks: \"\"\\\".\"\"\"", 132, 3, 8 },
        { TOKEN_KEY, nullptr, "str6", 176, 4, 1 }, { TOKEN_VALUE, new StringValue("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"."), "\"\"\"Here are fifteen quotation marks: \"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\".\"\"\"", 183, 4, 8 },
        { TOKEN_KEY, nullptr, "str7", 299, 7, 1 }, { TOKEN_VALUE, new StringValue("\"This,\" she said, \"is just a pointless statement.\""), "\"\"\"\"This,\" she said, \"is just a pointless statement.\"\"\"\"", 306, 7, 8 },
        { TOKEN_EOF, nullptr, "", 363, 8, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "winpath", 32, 2, 1 }, { TOKEN_VALUE, new StringValue("C:\\Users\\nodejs\\templates"), "'C:\\Users\\nodejs\\templates'", 43, 2, 12 },
        { TOKEN_KEY, nullptr, "winpath2", 71, 3, 1 }, { TOKEN_VALUE, new StringValue("\\\\ServerX\\admin$\\system32\\"), "'\\\\ServerX\\admin$\\system32\\'", 82, 3, 12 },
        { TOKEN_KEY, nullptr, "quoted", 111, 4, 1 }, { TOKEN_VALUE, new StringValue("Tom \"Dubs\" Preston-Werner"), "'Tom \"Dubs\" Preston-Werner'", 122, 4, 12 },
        { TOKEN_KEY, nullptr, "regex", 150, 5, 1 }, { TOKEN_VALUE, new StringValue("<\\i\\c*\\s*>"), "'<\\i\\c*\\s*>'", 161, 5, 12 },
        { TOKEN_EOF, nullptr, "", 174, 6, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "regex2", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("I [dw]on't need \\d{2} apples"), "'''I [dw]on't need \\d{2} apples'''", 9, 1, 10 },
        { TOKEN_KEY, nullptr, "lines", 44, 2, 1 }, { TOKEN_VALUE, new StringValue("The first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n"), "'''\nThe first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n'''", 53, 2, 10 },
        { TOKEN_EOF, nullptr, "", 147, 8, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, multiline_literal_string_escapes)
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "quot15", 0, 1, 1 }, { TOKEN_VALUE, new StringValue("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\""), "'''Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"'''", 9, 1, 10 },
        { TOKEN_KEY, nullptr, "apos15", 140, 4, 1 }, { TOKEN_VALUE, new StringValue("Here are fifteen apostrophes: '''''''''''''''"), "\"Here are fifteen apostrophes: '''''''''''''''\"", 149, 4, 10 },
        { TOKEN_KEY, nullptr, "str", 240, 7, 1}, { TOKEN_VALUE, new StringValue("'That,' she said, 'is still pointless.'"), "''''That,' she said, 'is still pointless.''''", 246, 7, 7 },
        { TOKEN_EOF, nullptr, "", 292, 8, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "int1", 0, 1, 1 }, { TOKEN_VALUE, new IntegerValue(99), "+99", 7, 1, 8 },
        { TOKEN_KEY, nullptr, "int2", 11, 2, 1 }, { TOKEN_VALUE, new IntegerValue(42), "42", 18, 2, 8 },
        { TOKEN_KEY, nullptr, "int3", 21, 3, 1 }, { TOKEN_VALUE, new IntegerValue(0), "0", 28, 3, 8 },
        { TOKEN_KEY, nullptr, "int4", 30, 4, 1 }, { TOKEN_VALUE, new IntegerValue(-17), "-17", 37, 4, 8 },
        { TOKEN_KEY, nullptr, "int5", 41, 5, 1 }, { TOKEN_VALUE, new IntegerValue(1000), "1_000", 48, 5, 8 },
        { TOKEN_KEY, nullptr, "int6", 54, 6, 1 }, { TOKEN_VALUE, new IntegerValue(5349221), "5_349_221", 61, 6, 8 },
        { TOKEN_KEY, nullptr, "int7", 71, 7, 1 }, { TOKEN_VALUE, new IntegerValue(5349221), "53_49_221", 78, 7, 8 },
        { TOKEN_KEY, nullptr, "int8", 121, 8, 1 }, { TOKEN_VALUE, new IntegerValue(12345), "1_2_3_4_5", 128, 8, 8 },
        { TOKEN_KEY, nullptr, "hex1", 195, 11, 1 }, { TOKEN_VALUE, new IntegerValue(0xdeadbeef), "0xDEADBEEF", 202, 11, 8 },
        { TOKEN_KEY, nullptr, "hex2", 213, 12, 1 }, { TOKEN_VALUE, new IntegerValue(0xdeadbeef), "0xdeadbeef", 220, 12, 8 },
        { TOKEN_KEY, nullptr, "hex3", 231, 13, 1 }, { TOKEN_VALUE, new IntegerValue(0xdeadbeef), "0xdead_beef", 238, 13, 8 },
        { TOKEN_KEY, nullptr, "oct1", 276, 16, 1 }, { TOKEN_VALUE, new IntegerValue(01234567), "0o01234567", 283, 16, 8 },
        { TOKEN_KEY, nullptr, "oct2", 294, 17, 1 }, { TOKEN_VALUE, new IntegerValue(0755), "0o755", 301, 17, 8 },
        { TOKEN_KEY, nullptr, "bin1", 369, 20, 1 }, { TOKEN_VALUE, new IntegerValue(214), "0b11010110", 376, 20, 8 },
        { TOKEN_EOF, nullptr, "", 387, 21, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "flt1", 13, 2, 1 }, { TOKEN_VALUE, new FloatValue(1), "+1.0", 20, 2, 8 },
        { TOKEN_KEY, nullptr, "flt2", 25, 3, 1 }, { TOKEN_VALUE, new FloatValue(3.1415), "3.1415", 32, 3, 8 },
        { TOKEN_KEY, nullptr, "flt3", 39, 4, 1 }, { TOKEN_VALUE, new FloatValue(-0.01), "-0.01", 46, 4, 8 },
        { TOKEN_KEY, nullptr, "flt4", 64, 7, 1 }, { TOKEN_VALUE, new FloatValue(5e+22), "5e+22", 71, 7, 8 },
        { TOKEN_KEY, nullptr, "flt5", 77, 8, 1 }, { TOKEN_VALUE, new FloatValue(1e06), "1e06", 84, 8, 8 },
        { TOKEN_KEY, nullptr, "flt6", 89, 9, 1 }, { TOKEN_VALUE, new FloatValue(-2e-2), "-2E-2", 96, 9, 8},
        { TOKEN_KEY, nullptr, "flt7", 110, 12, 1 }, { TOKEN_VALUE, new FloatValue(6.626e-34), "6.626e-34", 117, 12, 8 },
        { TOKEN_KEY, nullptr, "flt8", 128, 14, 1 }, { TOKEN_VALUE, new FloatValue(224617.445991228), "224_617.445_991_228", 135, 14, 8 },
        { TOKEN_EOF, nullptr, "", 155, 15, 1 },
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
        { 2, 19, "Missing whole part of decimal number." },
        { 3, 21, "Missing fractional part of decimal number." },
        { 4, 21, "Missing fractional part of decimal number." },
    };

    assert_lex_errors(toml, errors);
}


TEST(lex, infinity)
{
    const string toml =
        "# infinity\n"
        "sf1 = inf  # positive infinity\n"
        "sf2 = +inf # positive infinity\n"
        "sf3 = -inf # negative infinity\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "sf1", 11, 2, 1 }, { TOKEN_VALUE, new FloatValue(INF64), "inf", 17, 2, 7 },
        { TOKEN_KEY, nullptr, "sf2", 42, 3, 1 }, { TOKEN_VALUE, new FloatValue(+INF64), "+inf", 48, 3, 7 },
        { TOKEN_KEY, nullptr, "sf3", 73, 4, 1 }, { TOKEN_VALUE, new FloatValue(-INF64), "-inf", 79, 4, 7 },
        { TOKEN_EOF, nullptr, "", 104, 5, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "sf4", 15, 2, 1 }, { TOKEN_VALUE, new FloatValue(NAN64), "nan", 21, 2, 7 },
        { TOKEN_KEY, nullptr, "sf5", 81, 3, 1 }, { TOKEN_VALUE, new FloatValue(+NAN64), "+nan", 87, 3, 7 },
        { TOKEN_KEY, nullptr, "sf6", 108, 4, 1 }, { TOKEN_VALUE, new FloatValue(-NAN64), "-nan", 114, 4, 7 },
        { TOKEN_EOF, nullptr, "", 171, 5, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, booleans)
{
    const string toml =
        "bool1 = true\n"
        "bool2 = false\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "bool1", 0, 1, 1}, { TOKEN_VALUE, new BooleanValue(true), "true", 8, 1, 9 },
        { TOKEN_KEY, nullptr, "bool2", 13, 2, 1}, { TOKEN_VALUE, new BooleanValue(false), "false", 21, 2, 9 },
        { TOKEN_EOF, nullptr, "", 27, 3, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, local_times)
{
    const string toml =
        "lt1 = 07:32:00\n"
        "lt2 = 00:32:00.999999\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "lt1", 0, 1, 1 }, { TOKEN_VALUE, new LocalTimeValue{LocalTime{chrono::hours{7} + chrono::minutes{32}}}, "07:32:00", 6, 1, 7 },
        { TOKEN_KEY, nullptr, "lt2", 15, 2, 1 }, { TOKEN_VALUE, new LocalTimeValue{LocalTime{chrono::minutes{32} + chrono::microseconds{999999}}}, "00:32:00.999999", 21, 2, 7 },
        { TOKEN_EOF, nullptr, "", 37, 3, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, local_dates)
{
    const string toml = "ld1 = 1979-05-27";

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "ld1", 0, 1, 1 }, { TOKEN_VALUE, new LocalDateValue{LocalDate{date::year{1979} / date::month{5} / date::day{27}}}, "1979-05-27", 6, 1, 7 },
        { TOKEN_EOF, nullptr, "", 16, 1, 17 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, local_datetimes)
{
    const string toml =
        "ldt1 = 1979-05-27T07:32:00\n"
        "ldt2 = 1979-05-27T00:32:00.999999\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "ldt1", 0, 1, 1 },
        { TOKEN_VALUE,
            new LocalDateTimeValue{
                LocalDate{date::year{1979} / date::month{5} / date::day{27}}
                + chrono::hours{7} + chrono::minutes{32}
            },
            "1979-05-27T07:32:00", 7, 1, 8},

        { TOKEN_KEY, nullptr, "ldt2", 27, 2, 1 },
        { TOKEN_VALUE,
            new LocalDateTimeValue{
                LocalDate{date::year{1979} / date::month{5} / date::day{27}}
                + chrono::minutes{32} + chrono::microseconds{999999}
            },
            "1979-05-27T00:32:00.999999", 34, 2, 8 },

        { TOKEN_EOF, nullptr, "", 61, 3, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "odt1", 0, 1, 1 },
        { TOKEN_VALUE,
            new OffsetDateTimeValue(
                    date::sys_days{date::year{1979} / date::month{5} / date::day{27}}
                    + chrono::hours{7} + chrono::minutes{32}),
            "1979-05-27T07:32:00Z", 7, 1, 8},

        { TOKEN_KEY, nullptr, "odt2", 28, 2, 1 },
        { TOKEN_VALUE,
            new OffsetDateTimeValue(
                    date::sys_days{date::year{1979} / date::month{5} / date::day{27}}
                    + chrono::hours{7} + chrono::minutes{32}),
            "1979-05-27T00:32:00-07:00", 35, 2, 8},

        { TOKEN_KEY, nullptr, "odt3", 61, 3, 1 },
        { TOKEN_VALUE,
            new OffsetDateTimeValue(
                    date::sys_days{date::year{1979} / date::month{5} / date::day{27}}
                    + chrono::hours{7} + chrono::minutes{32} + chrono::microseconds{999999}),
            "1979-05-27T00:32:00.999999-07:00", 68, 3, 8},


        { TOKEN_KEY, nullptr, "odt4", 101, 4, 1 },
        { TOKEN_VALUE,
            new OffsetDateTimeValue(
                    date::sys_days{date::year{1979} / date::month{5} / date::day{27}}
                    + chrono::hours{7} + chrono::minutes{32}),
            "1979-05-27 07:32:00Z", 108, 4, 8},

        { TOKEN_EOF, nullptr, "", 129, 5, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "integers", 0, 1, 1 }, { TOKEN_LBRACKET, nullptr, "", 11, 1, 12 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 13, 1, 14 }, { TOKEN_COMMA, nullptr, "", 14, 1, 15 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 16, 1, 17 }, { TOKEN_COMMA, nullptr, "", 17, 1, 18 },
            { TOKEN_VALUE, new IntegerValue(3), "3", 19, 1, 20 }, { TOKEN_RBRACKET, nullptr, "", 21, 1, 22 },
        { TOKEN_KEY, nullptr, "colors", 23, 2, 1 }, { TOKEN_LBRACKET, nullptr, "", 32, 2, 10 },
            { TOKEN_VALUE, new StringValue("red"), "\"red\"", 34, 2, 12 }, { TOKEN_COMMA, nullptr, "", 39, 2, 17 },
            { TOKEN_VALUE, new StringValue("yellow"), "\"yellow\"", 41, 2, 19 }, { TOKEN_COMMA, nullptr, "", 49, 2, 27 },
            { TOKEN_VALUE, new StringValue("green"), "\"green\"", 51, 2, 29 }, { TOKEN_RBRACKET, nullptr, "", 59, 2, 37 },
        { TOKEN_KEY, nullptr, "nested_arrays_of_ints", 61, 3, 1 }, { TOKEN_LBRACKET, nullptr, "", 85, 3, 25 },
            { TOKEN_LBRACKET, nullptr, "", 87, 3, 27 },
                { TOKEN_VALUE, new IntegerValue(1), "1", 89, 3, 29 }, { TOKEN_COMMA, nullptr, "", 90, 3, 30 },
                { TOKEN_VALUE, new IntegerValue(2), "2", 92, 3, 32 },
            { TOKEN_RBRACKET, nullptr, "", 94, 3, 34 }, { TOKEN_COMMA, nullptr, "", 95, 3, 35 },
            { TOKEN_LBRACKET, nullptr, "", 97, 3, 37 },
                { TOKEN_VALUE, new IntegerValue(3), "3", 98, 3, 38 }, { TOKEN_COMMA, nullptr, "", 99, 3, 39 },
                { TOKEN_VALUE, new IntegerValue(4), "4", 101, 3, 41 }, { TOKEN_COMMA, nullptr, "", 102, 3, 42 },
                { TOKEN_VALUE, new IntegerValue(5), "5", 104, 3, 44 },
            { TOKEN_RBRACKET, nullptr, "", 105, 3, 45 },
        { TOKEN_RBRACKET, nullptr, "", 107, 3, 47 },
        { TOKEN_KEY, nullptr, "nested_mixed_array", 109, 4, 1 }, { TOKEN_LBRACKET, nullptr, "", 130, 4, 22 },
            { TOKEN_LBRACKET, nullptr, "", 132, 4, 24 },
                { TOKEN_VALUE, new IntegerValue(1), "1", 134, 4, 26 }, { TOKEN_COMMA, nullptr, "", 135, 4, 27 },
                { TOKEN_VALUE, new IntegerValue(2), "2", 137, 4, 29 },
            { TOKEN_RBRACKET, nullptr, "", 139, 4, 31 }, { TOKEN_COMMA, nullptr, "", 140, 4, 32 },
            { TOKEN_LBRACKET, nullptr, "", 142, 4, 34 },
                { TOKEN_VALUE, new StringValue("a"), "\"a\"", 143, 4, 35 }, { TOKEN_COMMA, nullptr, "", 146, 4, 38 },
                { TOKEN_VALUE, new StringValue("b"), "\"b\"", 148, 4, 40 }, { TOKEN_COMMA, nullptr, "", 151, 4, 43 },
                { TOKEN_VALUE, new StringValue("c"), "\"c\"", 153, 4, 45 },
            { TOKEN_RBRACKET, nullptr, "", 156, 4, 48 },
        { TOKEN_RBRACKET, nullptr, "", 158, 4, 50 },
        { TOKEN_KEY, nullptr, "string_array", 160, 5, 1 }, { TOKEN_LBRACKET, nullptr, "", 175, 5, 16 },
            { TOKEN_VALUE, new StringValue("all"), "\"all\"", 177, 5, 18 }, { TOKEN_COMMA, nullptr, "", 182, 5, 23 },
            { TOKEN_VALUE, new StringValue("strings"), "'strings'", 184, 5, 25 }, { TOKEN_COMMA, nullptr, "", 193, 5, 34 },
            { TOKEN_VALUE, new StringValue("are the same"), "\"\"\"are the same\"\"\"", 195, 5, 36 }, { TOKEN_COMMA, nullptr, "", 213, 5, 54 },
            { TOKEN_VALUE, new StringValue("type"), "'''type'''", 215, 5, 56 }, { TOKEN_RBRACKET, nullptr, "", 226, 5, 67 },
        { TOKEN_KEY, nullptr, "numbers", 261, 8, 1 }, { TOKEN_LBRACKET, nullptr, "", 271, 8, 11 },
            { TOKEN_VALUE, new FloatValue(0.1), "0.1", 273, 8, 13 }, { TOKEN_COMMA, nullptr, "", 276, 8, 16 },
            { TOKEN_VALUE, new FloatValue(0.2), "0.2", 278, 8, 18 }, { TOKEN_COMMA, nullptr, "", 281, 8, 21 },
            { TOKEN_VALUE, new FloatValue(0.5), "0.5", 283, 8, 23 }, { TOKEN_COMMA, nullptr, "", 286, 8, 26 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 288, 8, 28 }, { TOKEN_COMMA, nullptr, "", 289, 8, 29 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 291, 8, 31 }, { TOKEN_COMMA, nullptr, "", 292, 8, 32 },
            { TOKEN_VALUE, new IntegerValue(5), "5", 294, 8, 34 }, { TOKEN_RBRACKET, nullptr, "", 296, 8, 36 },
        { TOKEN_KEY, nullptr, "contributors", 298, 9, 1 }, { TOKEN_LBRACKET, nullptr, "", 313, 9, 16 },
            { TOKEN_VALUE, new StringValue("Foo Bar <foo@example.com>"), "\"Foo Bar <foo@example.com>\"", 317, 10, 3 }, { TOKEN_COMMA, nullptr, "", 344, 10, 30 },
            { TOKEN_LBRACE, nullptr, "", 348, 11, 3 },
                { TOKEN_KEY, nullptr, "name", 350, 11, 5 }, { TOKEN_VALUE, new StringValue("Baz Qux"), "\"Baz Qux\"", 357, 11, 12 }, { TOKEN_COMMA, nullptr, "", 366, 11, 21},
                { TOKEN_KEY, nullptr, "email", 368, 11, 23 }, { TOKEN_VALUE, new StringValue("bazqux@example.com"), "\"bazqux@example.com\"", 376, 11, 31 }, { TOKEN_COMMA, nullptr, "", 396, 11, 51},
                { TOKEN_KEY, nullptr, "url", 398, 11, 53 }, { TOKEN_VALUE, new StringValue("https://example.com/bazqux"), "\"https://example.com/bazqux\"", 404, 11, 59 }, { TOKEN_RBRACE, nullptr, "", 433, 11, 88},
            { TOKEN_RBRACKET, nullptr, "", 435, 12, 1 },
        { TOKEN_EOF, nullptr, "", 437, 13, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, multiline_arrays)
{
    const string toml =
        "integers2 = [\n"
        "  1, 2, 3\n"
        "]\n"
        "\n"
        "integers3 = [\n"
        "  1,\n"
        "  2, # this is ok\n"
        "]\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "integers2", 0, 1, 1 }, { TOKEN_LBRACKET, nullptr, "", 12, 1, 13 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 16, 2, 3 }, { TOKEN_COMMA, nullptr, "", 17, 2, 4 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 19, 2, 6 }, { TOKEN_COMMA, nullptr, "", 20, 2, 7 },
            { TOKEN_VALUE, new IntegerValue(3), "3", 22, 2, 9 }, { TOKEN_RBRACKET, nullptr, "", 24, 3, 1 },
        { TOKEN_KEY, nullptr, "integers3", 27, 5, 1 }, { TOKEN_LBRACKET, nullptr, "", 39, 5, 13 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 43, 6, 3 }, { TOKEN_COMMA, nullptr, "", 44, 6, 4 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 48, 7, 3 }, { TOKEN_COMMA, nullptr, "", 49, 7, 4 },
            { TOKEN_RBRACKET, nullptr, "", 64, 8, 1 },
        { TOKEN_EOF, nullptr, "", 66, 9, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "name", 0, 1, 1 }, { TOKEN_LBRACE, nullptr, "", 7, 1, 8},
            { TOKEN_KEY, nullptr, "first", 9, 1, 10 }, { TOKEN_VALUE, new StringValue("Tom"), "\"Tom\"", 17, 1, 18 }, { TOKEN_COMMA, nullptr, "", 22, 1, 23 },
            { TOKEN_KEY, nullptr, "last", 24, 1, 25 }, { TOKEN_VALUE, new StringValue("Preston-Werner"), "\"Preston-Werner\"", 31, 1, 32 }, { TOKEN_RBRACE, nullptr, "", 48, 1, 49 },
        { TOKEN_KEY, nullptr, "point", 50, 2, 1 }, { TOKEN_LBRACE, nullptr, "", 58, 2, 9},
            { TOKEN_KEY, nullptr, "x", 60, 2, 11 }, { TOKEN_VALUE, new IntegerValue(1), "1", 64, 2, 15 }, { TOKEN_COMMA, nullptr, "", 65, 2, 16 },
            { TOKEN_KEY, nullptr, "y", 67, 2, 18 }, { TOKEN_VALUE, new IntegerValue(2), "2", 71, 2, 22 }, { TOKEN_RBRACE, nullptr, "", 73, 2, 24 },
        { TOKEN_KEY, nullptr, "animal", 75, 3, 1 }, { TOKEN_LBRACE, nullptr, "", 84, 3, 10},
            { TOKEN_KEY, nullptr, "type", 86, 3, 12 }, { TOKEN_KEY, nullptr, "name", 91, 3, 17 }, { TOKEN_VALUE, new StringValue("pug"), "\"pug\"", 98, 3, 24 },
            { TOKEN_RBRACE, nullptr, "", 104, 3, 30 },
        { TOKEN_EOF, nullptr, "", 106, 4, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, tables)
{
    const string toml =
        "[table]\n"
        "\n"
        "[table-1]\n"
        "key1 = \"some string\"\n"
        "key2 = 123\n"
        "\n"
        "[table-2]\n"
        "key1 = \"another string\"\n"
        "key2 = 456\n"
        "\n"
        "[dog.\"tater.man\"]\n"
        "type.name = \"pug\"\n"
        ;

    vector<Token> tokens = {
        { TOKEN_LBRACKET, nullptr, "", 0, 1, 1 }, { TOKEN_KEY, nullptr, "table", 1, 1, 2 }, { TOKEN_RBRACKET, nullptr, "", 6, 1, 7 },
        { TOKEN_LBRACKET, nullptr, "", 9, 3, 1 }, { TOKEN_KEY, nullptr, "table-1", 10, 3, 2 }, { TOKEN_RBRACKET, nullptr, "", 17, 3, 9 },
        { TOKEN_KEY, nullptr, "key1", 19, 4, 1 }, { TOKEN_VALUE, new StringValue("some string"), "\"some string\"", 26, 4, 8 },
        { TOKEN_KEY, nullptr, "key2", 40, 5, 1 }, { TOKEN_VALUE, new IntegerValue(123), "123", 47, 5, 8 },
        { TOKEN_LBRACKET, nullptr, "", 52, 7, 1 }, { TOKEN_KEY, nullptr, "table-2", 53, 7, 2 }, { TOKEN_RBRACKET, nullptr, "", 60, 7, 9 },
        { TOKEN_KEY, nullptr, "key1", 62, 8, 1 }, { TOKEN_VALUE, new StringValue("another string"), "\"another string\"", 69, 8, 8 },
        { TOKEN_KEY, nullptr, "key2", 86, 9, 1 }, { TOKEN_VALUE, new IntegerValue(456), "456", 93, 9, 8 },
        { TOKEN_LBRACKET, nullptr, "", 98, 11, 1 }, { TOKEN_KEY, nullptr, "dog", 99, 11, 2 }, { TOKEN_KEY, nullptr, "tater.man", 103, 11, 6 }, { TOKEN_RBRACKET, nullptr, "", 114, 11, 17 },
        { TOKEN_KEY, nullptr, "type", 116, 12, 1 }, {TOKEN_KEY, nullptr, "name", 121, 12, 6 }, { TOKEN_VALUE, new StringValue("pug"), "\"pug\"", 128, 12, 13 },
        { TOKEN_EOF, nullptr, "", 134, 13, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, spaces_in_table_headers)
{
    const string toml =
        "[a.b.c]            # this is best practice\n"
        "[ d.e.f ]          # same as [d.e.f]\n"
        "[ g .  h  . i ]    # same as [g.h.i]\n"
        "[ j . \"ʞ\" . 'l' ]  # same as [j.\"ʞ\".'l']\n"
        ;

    vector<Token> tokens = {
        { TOKEN_LBRACKET, nullptr, "", 0, 1, 1 }, { TOKEN_KEY, nullptr, "a", 1, 1, 2 }, { TOKEN_KEY, nullptr, "b", 3, 1, 4 }, { TOKEN_KEY, nullptr, "c", 5, 1, 6 }, { TOKEN_RBRACKET, nullptr, "", 6, 1, 7 },
        { TOKEN_LBRACKET, nullptr, "", 43, 2, 1 }, { TOKEN_KEY, nullptr, "d", 45, 2, 3 }, { TOKEN_KEY, nullptr, "e", 47, 2, 5 }, { TOKEN_KEY, nullptr, "f", 49, 2, 7 }, { TOKEN_RBRACKET, nullptr, "", 51, 2, 9 },
        { TOKEN_LBRACKET, nullptr, "", 80, 3, 1 }, { TOKEN_KEY, nullptr, "g", 82, 3, 3 }, { TOKEN_KEY, nullptr, "h", 87, 3, 8 }, { TOKEN_KEY, nullptr, "i", 92, 3, 13 }, { TOKEN_RBRACKET, nullptr, "", 94, 3, 15 },
        { TOKEN_LBRACKET, nullptr, "", 117, 4, 1 }, { TOKEN_KEY, nullptr, "j", 119, 4, 3 }, { TOKEN_KEY, nullptr, "ʞ", 123, 4, 7 }, { TOKEN_KEY, nullptr, "l", 130, 4, 13 }, { TOKEN_RBRACKET, nullptr, "", 134, 4, 17 },
        { TOKEN_EOF, nullptr, "", 160, 5, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, implicit_super_tables)
{
    const string toml =
        "# [x] you\n"
        "# [x.y] don't\n"
        "# [x.y.z] need these\n"
        "[x.y.z.w] # for this to work\n"
        "\n"
        "[x] # defining a super-table afterward is ok\n"
        ;

    vector<Token> tokens = {
        { TOKEN_LBRACKET, nullptr, "", 45, 4, 1 }, { TOKEN_KEY, nullptr, "x", 46, 4, 2 }, { TOKEN_KEY, nullptr, "y", 48, 4, 4 }, { TOKEN_KEY, nullptr, "z", 50, 4, 6 }, { TOKEN_KEY, nullptr, "w", 52, 4, 8 }, { TOKEN_RBRACKET, nullptr, "", 53, 4, 9 },
        { TOKEN_LBRACKET, nullptr, "", 75, 6, 1 }, { TOKEN_KEY, nullptr, "x", 76, 6, 2 }, { TOKEN_RBRACKET, nullptr, "", 77, 6, 3 },
        { TOKEN_EOF, nullptr, "", 120, 7, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, table_arrays)
{
    const string toml =
        "[[products]]\n"
        "name = \"Hammer\"\n"
        "sku = 738594937\n"
        "\n"
        "[[products]]  # empty table within the array\n"
        "\n"
        "[[products]]\n"
        "name = \"Nail\"\n"
        "sku = 284758393\n"
        "\n"
        "color = \"gray\"\n"
        ;

    vector<Token> tokens = {
        { TOKEN_DOUBLE_LBRACKET, nullptr, "", 0, 1, 1 }, { TOKEN_KEY, nullptr, "products", 2, 1, 3 }, { TOKEN_DOUBLE_RBRACKET, nullptr, "", 10, 1, 11 },
        { TOKEN_KEY, nullptr, "name", 13, 2, 1 }, { TOKEN_VALUE, new StringValue("Hammer"), "\"Hammer\"", 20, 2, 8 },
        { TOKEN_KEY, nullptr, "sku", 29, 3, 1 }, { TOKEN_VALUE, new IntegerValue(738594937), "738594937", 35, 3, 7 },
        { TOKEN_DOUBLE_LBRACKET, nullptr, "", 46, 5, 1 }, { TOKEN_KEY, nullptr, "products", 48, 5, 3 }, { TOKEN_DOUBLE_RBRACKET, nullptr, "", 56, 5, 11 },
        { TOKEN_DOUBLE_LBRACKET, nullptr, "", 92, 7, 1 }, { TOKEN_KEY, nullptr, "products", 94, 7, 3 }, { TOKEN_DOUBLE_RBRACKET, nullptr, "", 102, 7, 11 },
        { TOKEN_KEY, nullptr, "name", 105, 8, 1 }, { TOKEN_VALUE, new StringValue("Nail"), "\"Nail\"", 112, 8, 8 },
        { TOKEN_KEY, nullptr, "sku", 119, 9, 1 }, { TOKEN_VALUE, new IntegerValue(284758393), "284758393", 125, 9, 7 },
        { TOKEN_KEY, nullptr, "color", 136, 11, 1 }, { TOKEN_VALUE, new StringValue("gray"), "\"gray\"", 144, 11, 9 },
        { TOKEN_EOF, nullptr, "", 151, 12, 1 },
    };

    assert_lexed(toml, tokens);
}
