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
    os << "Token(" << token.line << ", " << token.column << ", " << TOKEN_NAMES[token.type] << ", \"" << token.lexeme << "\")";
    return os;
}


}


TEST(lex, comments)
{
    const string toml =
        "# This is a full-line comment\n"
        "key = \"value\"  # This is a comment at the end of a line\n"
        "another = \"# This is not a comment\"\n"
        ;

    vector<Token> expected = {
        {TOKEN_KEY, "key", 2, 1}, {TOKEN_STRING, "value", 2, 7},
        {TOKEN_KEY, "another", 3, 1}, {TOKEN_STRING, "# This is not a comment", 3, 11},
    };

    TokenList actual;
    bool result = lex_toml(toml, actual);


    ASSERT_TRUE(result);
    ASSERT_EQ(actual, expected);
}
