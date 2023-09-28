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
    TOKEN_COMMENT,
    TOKEN_DOUBLE_LBRACKET,
    TOKEN_DOUBLE_RBRACKET,
    TOKEN_EOF,
    TOKEN_EQUAL,
    TOKEN_KEY,
    TOKEN_LBRACE,
    TOKEN_LBRACKET,
    TOKEN_NEWLINE,
    TOKEN_PERIOD,
    TOKEN_RBRACE,
    TOKEN_RBRACKET,
    TOKEN_VALUE,
};


struct Token
{
    TokenType type;
    Value value;
    std::string lexeme;
    SourceLocation location;
};



}

#endif // TOML_TOKEN_HPP
