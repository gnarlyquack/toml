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
#include "types.hpp"


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


struct Lexer
{
    const std::string &toml;
    u64 length;

    SourceLocation current;
    SourceLocation start;

    std::vector<Error> &errors;


    explicit Lexer(const std::string &t, std::vector<Error> &error_list)
        : toml(t)
        , length(toml.length())
        , current({0, 1, 1})
        , start(current)
        , errors(error_list)
    {
    }
};


std::string
get_lexeme(const Lexer &lexer, u64 from);


Token
next_token(Lexer &lexer, u32 context);


void
resynchronize(Lexer &Lexer, u32 context);


} // namespace toml


#endif // TOML_LEXER_HPP
