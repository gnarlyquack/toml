//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TOKEN_HPP
#define TOML_TOKEN_HPP

#include <string>


namespace toml
{


enum TokenType
{
    TOKEN_ERROR,

    TOKEN_BINARY,
    TOKEN_COMMA,
    TOKEN_DAY,
    TOKEN_DECIMAL,
    TOKEN_EXPONENT,
    TOKEN_FALSE,
    TOKEN_FRACTION,
    TOKEN_INF,
    TOKEN_KEY,
    TOKEN_HEXADECIMAL,
    TOKEN_HOUR,
    TOKEN_LBRACE,
    TOKEN_LBRACKET,
    TOKEN_MINUS,
    TOKEN_MINUTE,
    TOKEN_MONTH,
    TOKEN_NAN,
    TOKEN_NEWLINE,
    TOKEN_OCTAL,
    TOKEN_PLUS,
    TOKEN_RBRACE,
    TOKEN_RBRACKET,
    TOKEN_SECOND,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_YEAR,
};


struct Token
{
    TokenType type;
    std::string lexeme;

    std::string::size_type line;
    std::string::size_type column;
};



}

#endif // TOML_TOKEN_HPP
