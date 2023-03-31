//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include <gtest/gtest.h>

#include "test_common.hpp"


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
assert_errors(const string &toml, const vector<Error> &expected)
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
        { TOKEN_KEY, nullptr, "key", 2, 1 },
        { TOKEN_VALUE, new StringValue("value"), "\"value\"", 2, 7 },
        { TOKEN_KEY, nullptr, "another", 3, 1 },
        { TOKEN_VALUE, new StringValue("# This is not a comment"), "\"# This is not a comment\"", 3, 11 } ,
        { TOKEN_EOF, nullptr, "", 4, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, keyvals)
{
    const string toml = "key = \"value\"";

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "key", 1, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 1, 7 },
        { TOKEN_EOF, nullptr, "", 1, 14 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "key", 1, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 1, 7 },
        { TOKEN_KEY, nullptr, "bare_key", 2, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 2, 12 },
        { TOKEN_KEY, nullptr, "bare-key", 3, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 3, 12 },
        { TOKEN_KEY, nullptr, "1234", 4, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 4, 8 },
        { TOKEN_EOF, nullptr, "", 5, 1 },
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
        { TOKEN_KEY, nullptr, "127.0.0.1", 1, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 1, 15 },
        { TOKEN_KEY, nullptr, "character encoding", 2, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 2, 24 },
        { TOKEN_KEY, nullptr, "ʎǝʞ", 3, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 3, 9 },
        { TOKEN_KEY, nullptr, "key2", 4, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 4, 10 },
        { TOKEN_KEY, nullptr, "quoted \"value\"", 5, 1 }, { TOKEN_VALUE, new StringValue("value"), "\"value\"", 5, 20 },
        { TOKEN_EOF, nullptr, "", 6, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "", 1, 1 }, { TOKEN_VALUE, new StringValue("blank"), "\"blank\"", 1, 6 },
        { TOKEN_EOF, nullptr, "", 1, 41 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, empty_literal_key)
{
    const string toml = "'' = 'blank'     # VALID but discouraged";

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "", 1, 1 }, { TOKEN_VALUE, new StringValue("blank"), "'blank'", 1, 6 },
        { TOKEN_EOF, nullptr, "", 1, 41 },
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
        { TOKEN_KEY, nullptr, "name", 1, 1 }, { TOKEN_VALUE, new StringValue("Orange"), "\"Orange\"", 1, 8 },
        { TOKEN_KEY, nullptr, "physical", 2, 1 }, { TOKEN_KEY, nullptr, "color", 2, 10 }, { TOKEN_VALUE, new StringValue("orange"), "\"orange\"", 2, 18 },
        { TOKEN_KEY, nullptr, "physical", 3, 1 }, { TOKEN_KEY, nullptr, "shape", 3, 10 }, { TOKEN_VALUE, new StringValue("round"), "\"round\"", 3, 18 },
        { TOKEN_KEY, nullptr, "site", 4, 1 }, { TOKEN_KEY, nullptr, "google.com", 4, 6 }, { TOKEN_VALUE, new BooleanValue(true), "true", 4, 21 },
        { TOKEN_KEY, nullptr, "fruit", 5, 1 }, { TOKEN_KEY, nullptr, "name", 5, 7 }, { TOKEN_VALUE, new StringValue("banana"), "\"banana\"", 5, 14 },
        { TOKEN_KEY, nullptr, "fruit", 6, 1 }, { TOKEN_KEY, nullptr, "color", 6, 8 }, { TOKEN_VALUE, new StringValue("yellow"), "\"yellow\"", 6, 16 },
        { TOKEN_KEY, nullptr, "fruit", 7, 1 }, { TOKEN_KEY, nullptr, "flavor", 7, 9 }, { TOKEN_VALUE, new StringValue("banana"), "\"banana\"", 7, 18 },
        { TOKEN_KEY, nullptr, "3", 8, 1 }, { TOKEN_KEY, nullptr, "14159", 8, 3 }, { TOKEN_VALUE, new StringValue("pi"), "\"pi\"", 8, 11 },
        { TOKEN_EOF, nullptr, "", 9, 1 },
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
        { TOKEN_KEY, nullptr, "name", 2, 1 }, { TOKEN_VALUE, new StringValue("Tom"), "\"Tom\"", 2, 8 },
        { TOKEN_KEY, nullptr, "name", 3, 1 }, { TOKEN_VALUE, new StringValue("Pradyun"), "\"Pradyun\"", 3, 8 },
        { TOKEN_KEY, nullptr, "spelling", 5, 1 }, { TOKEN_VALUE, new StringValue("favorite"), "\"favorite\"", 5, 12 },
        { TOKEN_KEY, nullptr, "spelling", 6, 1 }, { TOKEN_VALUE, new StringValue("favourite"), "\"favourite\"", 6, 14 },
        { TOKEN_EOF, nullptr, "", 7, 1 },
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
        { TOKEN_KEY, nullptr, "fruit", 2, 1 }, { TOKEN_KEY, nullptr, "apple", 2, 7 }, { TOKEN_KEY, nullptr, "smooth", 2, 13 }, { TOKEN_VALUE, new BooleanValue(true), "true", 2, 22 },
        { TOKEN_KEY, nullptr, "fruit", 5, 1 }, { TOKEN_KEY, nullptr, "orange", 5, 7 }, { TOKEN_VALUE, new IntegerValue(2), "2", 5, 16 },
        { TOKEN_EOF, nullptr, "", 6, 1 },
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
        { TOKEN_KEY, nullptr, "fruit", 4, 1 }, { TOKEN_KEY, nullptr, "apple", 4, 7 }, { TOKEN_VALUE, new IntegerValue(1), "1", 4, 15 },
        { TOKEN_KEY, nullptr, "fruit", 8, 1 }, { TOKEN_KEY, nullptr, "apple", 8, 7 }, { TOKEN_KEY, nullptr, "smooth", 8, 13 }, { TOKEN_VALUE, new BooleanValue(true), "true", 8, 22 },
        { TOKEN_EOF, nullptr, "", 9, 1 },
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
        { TOKEN_KEY, nullptr, "apple", 3, 1 }, { TOKEN_KEY, nullptr, "type", 3, 7 }, { TOKEN_VALUE, new StringValue("fruit"), "\"fruit\"", 3, 14 },
        { TOKEN_KEY, nullptr, "orange", 4, 1 }, { TOKEN_KEY, nullptr, "type", 4, 8 }, { TOKEN_VALUE, new StringValue("fruit"), "\"fruit\"", 4, 15 },
        { TOKEN_KEY, nullptr, "apple", 6, 1 }, { TOKEN_KEY, nullptr, "skin", 6, 7 }, { TOKEN_VALUE, new StringValue("thin"), "\"thin\"", 6, 14 },
        { TOKEN_KEY, nullptr, "orange", 7, 1 }, { TOKEN_KEY, nullptr, "skin", 7, 8 }, { TOKEN_VALUE, new StringValue("thick"), "\"thick\"", 7, 15 },
        { TOKEN_KEY, nullptr, "apple", 9, 1 }, { TOKEN_KEY, nullptr, "color", 9, 7 }, { TOKEN_VALUE, new StringValue("red"), "\"red\"", 9, 15 },
        { TOKEN_KEY, nullptr, "orange", 10, 1 }, { TOKEN_KEY, nullptr, "color", 10, 8 }, { TOKEN_VALUE, new StringValue("orange"), "\"orange\"", 10, 16 },
        { TOKEN_EOF, nullptr, "", 11, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, basic_strings)
{
    const string toml =
        "str = \"I'm a string. \\\"You can quote me\\\". Name\\tJos\\u00e9\\nLocation\\tSF.\"\n"
        ;

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "str", 1, 1 }, { TOKEN_VALUE, new StringValue("I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF."), "\"I'm a string. \\\"You can quote me\\\". Name\\tJos\\u00e9\\nLocation\\tSF.\"", 1, 7 },
        { TOKEN_EOF, nullptr, "", 2, 1 },
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
        { TOKEN_KEY, nullptr, "str1", 1, 1 }, { TOKEN_VALUE, new StringValue("Roses are red\nViolets are blue"), "\"\"\"\nRoses are red\nViolets are blue\"\"\"", 1, 8 },
        { TOKEN_EOF, nullptr, "", 4, 1 },
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
        { TOKEN_KEY, nullptr, "str1", 2, 1 }, { TOKEN_VALUE, new StringValue("The quick brown fox jumps over the lazy dog."), "\"The quick brown fox jumps over the lazy dog.\"", 2, 8 },
        { TOKEN_KEY, nullptr, "str2", 4, 1 }, { TOKEN_VALUE, new StringValue("The quick brown fox jumps over the lazy dog."), "\"\"\"\nThe quick brown \\\n\n\n  fox jumps over \\\n    the lazy dog.\"\"\"", 4, 8 },
        { TOKEN_KEY, nullptr, "str3", 11, 1 }, { TOKEN_VALUE, new StringValue("The quick brown fox jumps over the lazy dog."), "\"\"\"\\\n       The quick brown \\\n       fox jumps over \\\n       the lazy dog.\\\n       \"\"\"", 11, 8 },
        { TOKEN_EOF, nullptr, "", 16, 1 },
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
        { TOKEN_KEY, nullptr, "str4", 1, 1 }, { TOKEN_VALUE, new StringValue("Here are two quotation marks: \"\". Simple enough."), "\"\"\"Here are two quotation marks: \"\". Simple enough.\"\"\"", 1, 8 },
        { TOKEN_KEY, nullptr, "str5", 3, 1 }, { TOKEN_VALUE, new StringValue("Here are three quotation marks: \"\"\"."), "\"\"\"Here are three quotation marks: \"\"\\\".\"\"\"", 3, 8 },
        { TOKEN_KEY, nullptr, "str6", 4, 1 }, { TOKEN_VALUE, new StringValue("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"."), "\"\"\"Here are fifteen quotation marks: \"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\".\"\"\"", 4, 8 },
        { TOKEN_KEY, nullptr, "str7", 7, 1 }, { TOKEN_VALUE, new StringValue("\"This,\" she said, \"is just a pointless statement.\""), "\"\"\"\"This,\" she said, \"is just a pointless statement.\"\"\"\"", 7, 8 },
        { TOKEN_EOF, nullptr, "", 8, 1 },
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
        { TOKEN_KEY, nullptr, "winpath", 2, 1 }, { TOKEN_VALUE, new StringValue("C:\\Users\\nodejs\\templates"), "'C:\\Users\\nodejs\\templates'", 2, 12 },
        { TOKEN_KEY, nullptr, "winpath2", 3, 1 }, { TOKEN_VALUE, new StringValue("\\\\ServerX\\admin$\\system32\\"), "'\\\\ServerX\\admin$\\system32\\'", 3, 12 },
        { TOKEN_KEY, nullptr, "quoted", 4, 1 }, { TOKEN_VALUE, new StringValue("Tom \"Dubs\" Preston-Werner"), "'Tom \"Dubs\" Preston-Werner'", 4, 12 },
        { TOKEN_KEY, nullptr, "regex", 5, 1 }, { TOKEN_VALUE, new StringValue("<\\i\\c*\\s*>"), "'<\\i\\c*\\s*>'", 5, 12 },
        { TOKEN_EOF, nullptr, "", 6, 1 },
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
        { TOKEN_KEY, nullptr, "regex2", 1, 1 }, { TOKEN_VALUE, new StringValue("I [dw]on't need \\d{2} apples"), "'''I [dw]on't need \\d{2} apples'''", 1, 10 },
        { TOKEN_KEY, nullptr, "lines", 2, 1 }, { TOKEN_VALUE, new StringValue("The first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n"), "'''\nThe first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n'''", 2, 10 },
        { TOKEN_EOF, nullptr, "", 8, 1 },
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
        { TOKEN_KEY, nullptr, "quot15", 1, 1 }, { TOKEN_VALUE, new StringValue("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\""), "'''Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"'''", 1, 10 },
        { TOKEN_KEY, nullptr, "apos15", 4, 1 }, { TOKEN_VALUE, new StringValue("Here are fifteen apostrophes: '''''''''''''''"), "\"Here are fifteen apostrophes: '''''''''''''''\"", 4, 10 },
        { TOKEN_KEY, nullptr, "str", 7, 1}, { TOKEN_VALUE, new StringValue("'That,' she said, 'is still pointless.'"), "''''That,' she said, 'is still pointless.''''", 7, 7 },
        { TOKEN_EOF, nullptr, "", 8, 1 },
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
        { TOKEN_KEY, nullptr, "int1", 1, 1 }, { TOKEN_VALUE, new IntegerValue(99), "+99", 1, 8 },
        { TOKEN_KEY, nullptr, "int2", 2, 1 }, { TOKEN_VALUE, new IntegerValue(42), "42", 2, 8 },
        { TOKEN_KEY, nullptr, "int3", 3, 1 }, { TOKEN_VALUE, new IntegerValue(0), "0", 3, 8 },
        { TOKEN_KEY, nullptr, "int4", 4, 1 }, { TOKEN_VALUE, new IntegerValue(-17), "-17", 4, 8 },
        { TOKEN_KEY, nullptr, "int5", 5, 1 }, { TOKEN_VALUE, new IntegerValue(1000), "1_000", 5, 8 },
        { TOKEN_KEY, nullptr, "int6", 6, 1 }, { TOKEN_VALUE, new IntegerValue(5349221), "5_349_221", 6, 8 },
        { TOKEN_KEY, nullptr, "int7", 7, 1 }, { TOKEN_VALUE, new IntegerValue(5349221), "53_49_221", 7, 8 },
        { TOKEN_KEY, nullptr, "int8", 8, 1 }, { TOKEN_VALUE, new IntegerValue(12345), "1_2_3_4_5", 8, 8 },
        { TOKEN_KEY, nullptr, "hex1", 11, 1 }, { TOKEN_VALUE, new IntegerValue(0xdeadbeef), "0xDEADBEEF", 11, 8 },
        { TOKEN_KEY, nullptr, "hex2", 12, 1 }, { TOKEN_VALUE, new IntegerValue(0xdeadbeef), "0xdeadbeef", 12, 8 },
        { TOKEN_KEY, nullptr, "hex3", 13, 1 }, { TOKEN_VALUE, new IntegerValue(0xdeadbeef), "0xdead_beef", 13, 8 },
        { TOKEN_KEY, nullptr, "oct1", 16, 1 }, { TOKEN_VALUE, new IntegerValue(01234567), "0o01234567", 16, 8 },
        { TOKEN_KEY, nullptr, "oct2", 17, 1 }, { TOKEN_VALUE, new IntegerValue(0755), "0o755", 17, 8 },
        { TOKEN_KEY, nullptr, "bin1", 20, 1 }, { TOKEN_VALUE, new IntegerValue(214), "0b11010110", 20, 8 },
        { TOKEN_EOF, nullptr, "", 21, 1 },
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
        { TOKEN_KEY, nullptr, "flt1", 2, 1 }, { TOKEN_VALUE, new FloatValue(1), "+1.0", 2, 8 },
        { TOKEN_KEY, nullptr, "flt2", 3, 1 }, { TOKEN_VALUE, new FloatValue(3.1415), "3.1415", 3, 8 },
        { TOKEN_KEY, nullptr, "flt3", 4, 1 }, { TOKEN_VALUE, new FloatValue(-0.01), "-0.01", 4, 8 },
        { TOKEN_KEY, nullptr, "flt4", 7, 1 }, { TOKEN_VALUE, new FloatValue(5e+22), "5e+22", 7, 8 },
        { TOKEN_KEY, nullptr, "flt5", 8, 1 }, { TOKEN_VALUE, new FloatValue(1e06), "1e06", 8, 8 },
        { TOKEN_KEY, nullptr, "flt6", 9, 1 }, { TOKEN_VALUE, new FloatValue(-2e-2), "-2E-2", 9, 8},
        { TOKEN_KEY, nullptr, "flt7", 12, 1 }, { TOKEN_VALUE, new FloatValue(6.626e-34), "6.626e-34", 12, 8 },
        { TOKEN_KEY, nullptr, "flt8", 14, 1 }, { TOKEN_VALUE, new FloatValue(224617.445991228), "224_617.445_991_228", 14, 8 },
        { TOKEN_EOF, nullptr, "", 15, 1 },
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

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "sf1", 2, 1 }, { TOKEN_VALUE, new FloatValue(INF64), "inf", 2, 7 },
        { TOKEN_KEY, nullptr, "sf2", 3, 1 }, { TOKEN_VALUE, new FloatValue(+INF64), "+inf", 3, 7 },
        { TOKEN_KEY, nullptr, "sf3", 4, 1 }, { TOKEN_VALUE, new FloatValue(-INF64), "-inf", 4, 7 },
        { TOKEN_EOF, nullptr, "", 5, 1 },
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
        { TOKEN_KEY, nullptr, "sf4", 2, 1 }, { TOKEN_VALUE, new FloatValue(NAN64), "nan", 2, 7 },
        { TOKEN_KEY, nullptr, "sf5", 3, 1 }, { TOKEN_VALUE, new FloatValue(+NAN64), "+nan", 3, 7 },
        { TOKEN_KEY, nullptr, "sf6", 4, 1 }, { TOKEN_VALUE, new FloatValue(-NAN64), "-nan", 4, 7 },
        { TOKEN_EOF, nullptr, "", 5, 1 },
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
        { TOKEN_KEY, nullptr, "bool1", 1, 1}, { TOKEN_VALUE, new BooleanValue(true), "true", 1, 9 },
        { TOKEN_KEY, nullptr, "bool2", 2, 1}, { TOKEN_VALUE, new BooleanValue(false), "false", 2, 9 },
        { TOKEN_EOF, nullptr, "", 3, 1 },
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
        { TOKEN_KEY, nullptr, "lt1", 1, 1 }, { TOKEN_HOUR, nullptr, "07", 1, 7 }, { TOKEN_MINUTE, nullptr, "32", 1, 10 }, { TOKEN_SECOND, nullptr, "00", 1, 13 },
        { TOKEN_KEY, nullptr, "lt2", 2, 1 }, { TOKEN_HOUR, nullptr, "00", 2, 7 }, { TOKEN_MINUTE, nullptr, "32", 2, 10 }, { TOKEN_SECOND, nullptr, "00", 2, 13 }, { TOKEN_FRACTION, nullptr, "999999", 2, 16 },
        { TOKEN_EOF, nullptr, "", 3, 1 },
    };

    assert_lexed(toml, tokens);
}


TEST(lex, local_dates)
{
    const string toml = "ld1 = 1979-05-27";

    vector<Token> tokens = {
        { TOKEN_KEY, nullptr, "ld1", 1, 1 }, { TOKEN_YEAR, nullptr, "1979", 1, 7 }, { TOKEN_MONTH, nullptr, "05", 1, 12 }, { TOKEN_DAY, nullptr, "27", 1, 15 },
        { TOKEN_EOF, nullptr, "", 1, 17 },
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
        { TOKEN_KEY, nullptr, "ldt1", 1, 1 }, { TOKEN_YEAR, nullptr, "1979", 1, 8}, { TOKEN_MONTH, nullptr, "05", 1, 13 }, { TOKEN_DAY, nullptr, "27", 1, 16 }, { TOKEN_HOUR, nullptr, "07", 1, 19}, { TOKEN_MINUTE, nullptr, "32", 1, 22 }, { TOKEN_SECOND, nullptr, "00", 1, 25 },
        { TOKEN_KEY, nullptr, "ldt2", 2, 1 }, { TOKEN_YEAR, nullptr, "1979", 2, 8}, { TOKEN_MONTH, nullptr, "05", 2, 13 }, { TOKEN_DAY, nullptr, "27", 2, 16 }, { TOKEN_HOUR, nullptr, "00", 2, 19}, { TOKEN_MINUTE, nullptr, "32", 2, 22 }, { TOKEN_SECOND, nullptr, "00", 2, 25 }, { TOKEN_FRACTION, nullptr, "999999", 2, 28 },
        { TOKEN_EOF, nullptr, "", 3, 1 },
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
        { TOKEN_KEY, nullptr, "odt1", 1, 1 }, { TOKEN_YEAR, nullptr, "1979", 1, 8}, { TOKEN_MONTH, nullptr, "05", 1, 13 }, { TOKEN_DAY, nullptr, "27", 1, 16 }, { TOKEN_HOUR, nullptr, "07", 1, 19}, { TOKEN_MINUTE, nullptr, "32", 1, 22 }, { TOKEN_SECOND, nullptr, "00", 1, 25 }, { TOKEN_PLUS, nullptr, "+", 1, 27 }, { TOKEN_HOUR, nullptr, "00", 1, 28 }, { TOKEN_MINUTE, nullptr, "00", 1, 28 },
        { TOKEN_KEY, nullptr, "odt2", 2, 1 }, { TOKEN_YEAR, nullptr, "1979", 2, 8}, { TOKEN_MONTH, nullptr, "05", 2, 13 }, { TOKEN_DAY, nullptr, "27", 2, 16 }, { TOKEN_HOUR, nullptr, "00", 2, 19}, { TOKEN_MINUTE, nullptr, "32", 2, 22 }, { TOKEN_SECOND, nullptr, "00", 2, 25 }, { TOKEN_MINUS, nullptr, "-", 2, 27 }, { TOKEN_HOUR, nullptr, "07", 2, 28 }, { TOKEN_MINUTE, nullptr, "00", 2, 31 },
        { TOKEN_KEY, nullptr, "odt3", 3, 1 }, { TOKEN_YEAR, nullptr, "1979", 3, 8}, { TOKEN_MONTH, nullptr, "05", 3, 13 }, { TOKEN_DAY, nullptr, "27", 3, 16 }, { TOKEN_HOUR, nullptr, "00", 3, 19}, { TOKEN_MINUTE, nullptr, "32", 3, 22 }, { TOKEN_SECOND, nullptr, "00", 3, 25 }, { TOKEN_FRACTION, nullptr, "999999", 3, 28 }, { TOKEN_MINUS, nullptr, "-", 3, 34 }, { TOKEN_HOUR, nullptr, "07", 3, 35 }, { TOKEN_MINUTE, nullptr, "00", 3, 38 },
        { TOKEN_KEY, nullptr, "odt4", 4, 1 }, { TOKEN_YEAR, nullptr, "1979", 4, 8}, { TOKEN_MONTH, nullptr, "05", 4, 13 }, { TOKEN_DAY, nullptr, "27", 4, 16 }, { TOKEN_HOUR, nullptr, "07", 4, 19}, { TOKEN_MINUTE, nullptr, "32", 4, 22 }, { TOKEN_SECOND, nullptr, "00", 4, 25 }, { TOKEN_PLUS, nullptr, "+", 4, 27 }, { TOKEN_HOUR, nullptr, "00", 4, 28 }, { TOKEN_MINUTE, nullptr, "00", 4, 28 },
        { TOKEN_EOF, nullptr, "", 5, 1 },
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
        { TOKEN_KEY, nullptr, "integers", 1, 1 }, { TOKEN_LBRACKET, nullptr, "", 1, 12 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 1, 14 }, { TOKEN_COMMA, nullptr, "", 1, 15 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 1, 17 }, { TOKEN_COMMA, nullptr, "", 1, 18 },
            { TOKEN_VALUE, new IntegerValue(3), "3", 1, 20 }, { TOKEN_RBRACKET, nullptr, "", 1, 22 },
        { TOKEN_KEY, nullptr, "colors", 2, 1 }, { TOKEN_LBRACKET, nullptr, "", 2, 10 },
            { TOKEN_VALUE, new StringValue("red"), "\"red\"", 2, 12 }, { TOKEN_COMMA, nullptr, "", 2, 17 },
            { TOKEN_VALUE, new StringValue("yellow"), "\"yellow\"", 2, 19 }, { TOKEN_COMMA, nullptr, "", 2, 27 },
            { TOKEN_VALUE, new StringValue("green"), "\"green\"", 2, 29 }, { TOKEN_RBRACKET, nullptr, "", 2, 37 },
        { TOKEN_KEY, nullptr, "nested_arrays_of_ints", 3, 1 }, { TOKEN_LBRACKET, nullptr, "", 3, 25 },
            { TOKEN_LBRACKET, nullptr, "", 3, 27 },
                { TOKEN_VALUE, new IntegerValue(1), "1", 3, 29 }, { TOKEN_COMMA, nullptr, "", 3, 30 },
                { TOKEN_VALUE, new IntegerValue(2), "2", 3, 32 },
            { TOKEN_RBRACKET, nullptr, "", 3, 34 }, { TOKEN_COMMA, nullptr, "", 3, 35 },
            { TOKEN_LBRACKET, nullptr, "", 3, 37 },
                { TOKEN_VALUE, new IntegerValue(3), "3", 3, 38 }, { TOKEN_COMMA, nullptr, "", 3, 39 },
                { TOKEN_VALUE, new IntegerValue(4), "4", 3, 41 }, { TOKEN_COMMA, nullptr, "", 3, 42 },
                { TOKEN_VALUE, new IntegerValue(5), "5", 3, 44 },
            { TOKEN_RBRACKET, nullptr, "", 3, 45 },
        { TOKEN_RBRACKET, nullptr, "", 3, 47 },
        { TOKEN_KEY, nullptr, "nested_mixed_array", 4, 1 }, { TOKEN_LBRACKET, nullptr, "", 4, 22 },
            { TOKEN_LBRACKET, nullptr, "", 4, 24 },
                { TOKEN_VALUE, new IntegerValue(1), "1", 4, 26 }, { TOKEN_COMMA, nullptr, "", 4, 27 },
                { TOKEN_VALUE, new IntegerValue(2), "2", 4, 29 },
            { TOKEN_RBRACKET, nullptr, "", 4, 31 }, { TOKEN_COMMA, nullptr, "", 4, 32 },
            { TOKEN_LBRACKET, nullptr, "", 4, 34 },
                { TOKEN_VALUE, new StringValue("a"), "\"a\"", 4, 35 }, { TOKEN_COMMA, nullptr, "", 4, 38 },
                { TOKEN_VALUE, new StringValue("b"), "\"b\"", 4, 40 }, { TOKEN_COMMA, nullptr, "", 4, 43 },
                { TOKEN_VALUE, new StringValue("c"), "\"c\"", 4, 45 },
            { TOKEN_RBRACKET, nullptr, "", 4, 48 },
        { TOKEN_RBRACKET, nullptr, "", 4, 50 },
        { TOKEN_KEY, nullptr, "string_array", 5, 1 }, { TOKEN_LBRACKET, nullptr, "", 5, 16 },
            { TOKEN_VALUE, new StringValue("all"), "\"all\"", 5, 18 }, { TOKEN_COMMA, nullptr, "", 5, 23 },
            { TOKEN_VALUE, new StringValue("strings"), "'strings'", 5, 25 }, { TOKEN_COMMA, nullptr, "", 5, 34 },
            { TOKEN_VALUE, new StringValue("are the same"), "\"\"\"are the same\"\"\"", 5, 36 }, { TOKEN_COMMA, nullptr, "", 5, 54 },
            { TOKEN_VALUE, new StringValue("type"), "'''type'''", 5, 56 }, { TOKEN_RBRACKET, nullptr, "", 5, 67 },
        { TOKEN_KEY, nullptr, "numbers", 8, 1 }, { TOKEN_LBRACKET, nullptr, "", 8, 11 },
            { TOKEN_VALUE, new FloatValue(0.1), "0.1", 8, 13 }, { TOKEN_COMMA, nullptr, "", 8, 16 },
            { TOKEN_VALUE, new FloatValue(0.2), "0.2", 8, 18 }, { TOKEN_COMMA, nullptr, "", 8, 21 },
            { TOKEN_VALUE, new FloatValue(0.5), "0.5", 8, 23 }, { TOKEN_COMMA, nullptr, "", 8, 26 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 8, 28 }, { TOKEN_COMMA, nullptr, "", 8, 29 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 8, 31 }, { TOKEN_COMMA, nullptr, "", 8, 32 },
            { TOKEN_VALUE, new IntegerValue(5), "5", 8, 34 }, { TOKEN_RBRACKET, nullptr, "", 8, 36 },
        { TOKEN_KEY, nullptr, "contributors", 9, 1 }, { TOKEN_LBRACKET, nullptr, "", 9, 16 },
            { TOKEN_VALUE, new StringValue("Foo Bar <foo@example.com>"), "\"Foo Bar <foo@example.com>\"", 10, 3 }, { TOKEN_COMMA, nullptr, "", 10, 30 },
            { TOKEN_LBRACE, nullptr, "", 11, 3 },
                { TOKEN_KEY, nullptr, "name", 11, 5 }, { TOKEN_VALUE, new StringValue("Baz Qux"), "\"Baz Qux\"", 11, 12 }, { TOKEN_COMMA, nullptr, "", 11, 21},
                { TOKEN_KEY, nullptr, "email", 11, 23 }, { TOKEN_VALUE, new StringValue("bazqux@example.com"), "\"bazqux@example.com\"", 11, 31 }, { TOKEN_COMMA, nullptr, "", 11, 51},
                { TOKEN_KEY, nullptr, "url", 11, 53 }, { TOKEN_VALUE, new StringValue("https://example.com/bazqux"), "\"https://example.com/bazqux\"", 11, 59 }, { TOKEN_RBRACE, nullptr, "", 11, 88},
            { TOKEN_RBRACKET, nullptr, "", 12, 1 },
        { TOKEN_EOF, nullptr, "", 13, 1 },
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
        { TOKEN_KEY, nullptr, "integers2", 1, 1 }, { TOKEN_LBRACKET, nullptr, "", 1, 13 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 2, 3 }, { TOKEN_COMMA, nullptr, "", 2, 4 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 2, 6 }, { TOKEN_COMMA, nullptr, "", 2, 7 },
            { TOKEN_VALUE, new IntegerValue(3), "3", 2, 9 }, { TOKEN_RBRACKET, nullptr, "", 3, 1 },
        { TOKEN_KEY, nullptr, "integers3", 5, 1 }, { TOKEN_LBRACKET, nullptr, "", 5, 13 },
            { TOKEN_VALUE, new IntegerValue(1), "1", 6, 3 }, { TOKEN_COMMA, nullptr, "", 6, 4 },
            { TOKEN_VALUE, new IntegerValue(2), "2", 7, 3 }, { TOKEN_COMMA, nullptr, "", 7, 4 },
            { TOKEN_RBRACKET, nullptr, "", 8, 1 },
        { TOKEN_EOF, nullptr, "", 9, 1 },
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
        { TOKEN_KEY, nullptr, "name", 1, 1 }, { TOKEN_LBRACE, nullptr, "", 1, 8},
            { TOKEN_KEY, nullptr, "first", 1, 10 }, { TOKEN_VALUE, new StringValue("Tom"), "\"Tom\"", 1, 18 }, { TOKEN_COMMA, nullptr, "", 1, 23 },
            { TOKEN_KEY, nullptr, "last", 1, 25 }, { TOKEN_VALUE, new StringValue("Preston-Werner"), "\"Preston-Werner\"", 1, 32 }, { TOKEN_RBRACE, nullptr, "", 1, 49 },
        { TOKEN_KEY, nullptr, "point", 2, 1 }, { TOKEN_LBRACE, nullptr, "", 2, 9},
            { TOKEN_KEY, nullptr, "x", 2, 11 }, { TOKEN_VALUE, new IntegerValue(1), "1", 2, 15 }, { TOKEN_COMMA, nullptr, "", 2, 16 },
            { TOKEN_KEY, nullptr, "y", 2, 18 }, { TOKEN_VALUE, new IntegerValue(2), "2", 2, 22 }, { TOKEN_RBRACE, nullptr, "", 2, 24 },
        { TOKEN_KEY, nullptr, "animal", 3, 1 }, { TOKEN_LBRACE, nullptr, "", 3, 10},
            { TOKEN_KEY, nullptr, "type", 3, 12 }, { TOKEN_KEY, nullptr, "name", 3, 17 }, { TOKEN_VALUE, new StringValue("pug"), "\"pug\"", 3, 24 },
            { TOKEN_RBRACE, nullptr, "", 3, 30 },
        { TOKEN_EOF, nullptr, "", 4, 1 },
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
        { TOKEN_LBRACKET, nullptr, "", 1, 1 }, { TOKEN_KEY, nullptr, "table", 1, 2 }, { TOKEN_RBRACKET, nullptr, "", 1, 7 },
        { TOKEN_LBRACKET, nullptr, "", 3, 1 }, { TOKEN_KEY, nullptr, "table-1", 3, 2 }, { TOKEN_RBRACKET, nullptr, "", 3, 9 },
        { TOKEN_KEY, nullptr, "key1", 4, 1 }, { TOKEN_VALUE, new StringValue("some string"), "\"some string\"", 4, 8 },
        { TOKEN_KEY, nullptr, "key2", 5, 1 }, { TOKEN_VALUE, new IntegerValue(123), "123", 5, 8 },
        { TOKEN_LBRACKET, nullptr, "", 7, 1 }, { TOKEN_KEY, nullptr, "table-2", 7, 2 }, { TOKEN_RBRACKET, nullptr, "", 7, 9 },
        { TOKEN_KEY, nullptr, "key1", 8, 1 }, { TOKEN_VALUE, new StringValue("another string"), "\"another string\"", 8, 8 },
        { TOKEN_KEY, nullptr, "key2", 9, 1 }, { TOKEN_VALUE, new IntegerValue(456), "456", 9, 8 },
        { TOKEN_LBRACKET, nullptr, "", 11, 1 }, { TOKEN_KEY, nullptr, "dog", 11, 2 }, { TOKEN_KEY, nullptr, "tater.man", 11, 6 }, { TOKEN_RBRACKET, nullptr, "", 11, 17 },
        { TOKEN_KEY, nullptr, "type", 12, 1 }, {TOKEN_KEY, nullptr, "name", 12, 6 }, { TOKEN_VALUE, new StringValue("pug"), "\"pug\"", 12, 13 },
        { TOKEN_EOF, nullptr, "", 13, 1 },
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
        { TOKEN_LBRACKET, nullptr, "", 1, 1 }, { TOKEN_KEY, nullptr, "a", 1, 2 }, { TOKEN_KEY, nullptr, "b", 1, 4 }, { TOKEN_KEY, nullptr, "c", 1, 6 }, { TOKEN_RBRACKET, nullptr, "", 1, 7 },
        { TOKEN_LBRACKET, nullptr, "", 2, 1 }, { TOKEN_KEY, nullptr, "d", 2, 3 }, { TOKEN_KEY, nullptr, "e", 2, 5 }, { TOKEN_KEY, nullptr, "f", 2, 7 }, { TOKEN_RBRACKET, nullptr, "", 2, 9 },
        { TOKEN_LBRACKET, nullptr, "", 3, 1 }, { TOKEN_KEY, nullptr, "g", 3, 3 }, { TOKEN_KEY, nullptr, "h", 3, 8 }, { TOKEN_KEY, nullptr, "i", 3, 13 }, { TOKEN_RBRACKET, nullptr, "", 3, 15 },
        { TOKEN_LBRACKET, nullptr, "", 4, 1 }, { TOKEN_KEY, nullptr, "j", 4, 3 }, { TOKEN_KEY, nullptr, "ʞ", 4, 7 }, { TOKEN_KEY, nullptr, "l", 4, 13 }, { TOKEN_RBRACKET, nullptr, "", 4, 17 },
        { TOKEN_EOF, nullptr, "", 5, 1 },
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
        { TOKEN_LBRACKET, nullptr, "", 4, 1 }, { TOKEN_KEY, nullptr, "x", 4, 2 }, { TOKEN_KEY, nullptr, "y", 4, 4 }, { TOKEN_KEY, nullptr, "z", 4, 6 }, { TOKEN_KEY, nullptr, "w", 4, 8 }, { TOKEN_RBRACKET, nullptr, "", 4, 9 },
        { TOKEN_LBRACKET, nullptr, "", 6, 1 }, { TOKEN_KEY, nullptr, "x", 6, 2 }, { TOKEN_RBRACKET, nullptr, "", 6, 3 },
        { TOKEN_EOF, nullptr, "", 7, 1 },
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
        { TOKEN_DOUBLE_LBRACKET, nullptr, "", 1, 1 }, { TOKEN_KEY, nullptr, "products", 1, 3 }, { TOKEN_DOUBLE_RBRACKET, nullptr, "", 1, 11 },
        { TOKEN_KEY, nullptr, "name", 2, 1 }, { TOKEN_VALUE, new StringValue("Hammer"), "\"Hammer\"", 2, 8 },
        { TOKEN_KEY, nullptr, "sku", 3, 1 }, { TOKEN_VALUE, new IntegerValue(738594937), "738594937", 3, 7 },
        { TOKEN_DOUBLE_LBRACKET, nullptr, "", 5, 1 }, { TOKEN_KEY, nullptr, "products", 5, 3 }, { TOKEN_DOUBLE_RBRACKET, nullptr, "", 5, 11 },
        { TOKEN_DOUBLE_LBRACKET, nullptr, "", 7, 1 }, { TOKEN_KEY, nullptr, "products", 7, 3 }, { TOKEN_DOUBLE_RBRACKET, nullptr, "", 7, 11 },
        { TOKEN_KEY, nullptr, "name", 8, 1 }, { TOKEN_VALUE, new StringValue("Nail"), "\"Nail\"", 8, 8 },
        { TOKEN_KEY, nullptr, "sku", 9, 1 }, { TOKEN_VALUE, new IntegerValue(284758393), "284758393", 9, 7 },
        { TOKEN_KEY, nullptr, "color", 11, 1 }, { TOKEN_VALUE, new StringValue("gray"), "\"gray\"", 11, 9 },
        { TOKEN_EOF, nullptr, "", 12, 1 },
    };

    assert_lexed(toml, tokens);
}
