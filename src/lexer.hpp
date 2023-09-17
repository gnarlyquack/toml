//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_LEXER_HPP
#define TOML_LEXER_HPP

#include <string>
#include <vector>

#include "error.hpp"
#include "token.hpp"


namespace toml
{


enum LexingContext
{
    LEX_EOL         =       0,
    LEX_FRACTION    = 1 <<  0,
    LEX_EXPONENT    = 1 <<  1,
    LEX_DATE        = 1 <<  2,
    LEX_TIME        = 1 <<  3,
    LEX_DATETIME    = 1 <<  4,
    LEX_TIMEZONE    = 1 <<  5,
    LEX_ARRAY       = 1 <<  6,
    LEX_TABLE       = 1 <<  7,
    LEX_HEADER      = 1 <<  8,
    LEX_KEY         = 1 <<  9,
    LEX_VALUE       = 1 << 10,
};


struct TomlIterator
{
    const std::string &toml;
    u64 length;

    u64 start_position;
    u32 start_line;
    u32 start_column;

    u64 current_position;
    u32 current_line;
    u32 current_column;

    std::vector<Token> &tokens;
    std::vector<Error> &errors;


    explicit TomlIterator(const std::string &t, std::vector<Token> &token_list, std::vector<Error> &error_list)
        : toml(t)
        , length(toml.length())
        , start_position(0)
        , start_line(1)
        , start_column(1)
        , current_position(0)
        , current_line(1)
        , current_column(1)
        , tokens(token_list)
        , errors(error_list)
    {
    }
};

using Lexer = TomlIterator;


Token
next_token(Lexer &lexer, u32 context);


void
resynchronize(Lexer &Lexer, std::string message, u32 context);


} // namespace toml


#endif // TOML_LEXER_HPP
