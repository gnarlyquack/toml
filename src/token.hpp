//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TOKEN_HPP
#define TOML_TOKEN_HPP

#include <string>

#include "common.hpp"
#include "types.hpp"


namespace toml
{


enum TokenType
{
    TOKEN_ERROR,

    TOKEN_COMMA,
    TOKEN_DAY,
    TOKEN_DOUBLE_LBRACKET,
    TOKEN_DOUBLE_RBRACKET,
    TOKEN_EOF,
    TOKEN_FRACTION,
    TOKEN_KEY,
    TOKEN_HOUR,
    TOKEN_LBRACE,
    TOKEN_LBRACKET,
    TOKEN_MINUS,
    TOKEN_MINUTE,
    TOKEN_MONTH,
    TOKEN_NEWLINE,
    TOKEN_PLUS,
    TOKEN_RBRACE,
    TOKEN_RBRACKET,
    TOKEN_SECOND,
    TOKEN_VALUE,
    TOKEN_YEAR,
};


struct Token
{
    TokenType type;
    Value value;
    std::string lexeme;
    u64 position;
    u32 line;
    u32 column;
};



}

#endif // TOML_TOKEN_HPP
