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
    "KEY",
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
        << token.lexeme << ", "
        << token.line << ", "
        << token.column  << "\")";
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

    ASSERT_TRUE(result);
    ASSERT_EQ(actual, expected);
    ASSERT_EQ(errors, vector<Error>{});
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
        {TOKEN_KEY, "key", 2, 1}, {TOKEN_STRING, "value", 2, 7},
        {TOKEN_KEY, "another", 3, 1}, {TOKEN_STRING, "# This is not a comment", 3, 11},
    };

    assert_lexed(toml, tokens);
}


TEST(lex, keyvals)
{
    const string toml = "key = \"value\"";

    const vector<Token> tokens = {
        {TOKEN_KEY, "key", 1, 1}, {TOKEN_STRING, "value", 1, 7},
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
        { TOKEN_KEY, "key", 1, 1 }, { TOKEN_STRING, "value", 1, 7},
        { TOKEN_KEY, "bare_key", 2, 1 }, { TOKEN_STRING, "value", 2, 12},
        { TOKEN_KEY, "bare-key", 3, 1 }, { TOKEN_STRING, "value", 3, 12},
        { TOKEN_KEY, "1234", 4, 1 }, { TOKEN_STRING, "value", 4, 8},
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
        { TOKEN_KEY, "127.0.0.1", 1, 1 }, { TOKEN_STRING, "value", 1, 15},
        { TOKEN_KEY, "character encoding", 2, 1 }, { TOKEN_STRING, "value", 2, 24},
        { TOKEN_KEY, "ʎǝʞ", 3, 1 }, { TOKEN_STRING, "value", 3, 9},
        { TOKEN_KEY, "key2", 4, 1 }, { TOKEN_STRING, "value", 4, 10},
        { TOKEN_KEY, "quoted \"value\"", 5, 1 }, { TOKEN_STRING, "value", 5, 20},
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
