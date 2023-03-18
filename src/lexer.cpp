//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include "lexer.hpp"

#include <cstdio> // fputs
#include <utility> // move

#include "common.hpp"


using namespace std;


namespace toml
{

namespace
{


// No UTF-8 byte can be 0xff
constexpr byte INVALID_BYTE = BYTE_MAX;


struct TomlIterator
{
    const string &toml;
    string::size_type position;
    string::size_type length;

    string::size_type start_line;
    string::size_type start_column;

    string::size_type current_line;
    string::size_type current_column;

    TokenList &tokens;
};


void
advance(TomlIterator &iterator)
{
    iterator.start_line = iterator.current_line;
    iterator.start_column = iterator.current_column;
}


void
add_token(TomlIterator &iterator, TokenType type, string lexeme)
{
    Token token = { type, move(lexeme), iterator.start_line, iterator.start_column };
    iterator.tokens.emplace_back(move(token));
    advance(iterator);
}


bool
end_of_file(const TomlIterator &iterator)
{
    assert(iterator.position <= iterator.length);
    bool result = iterator.position == iterator.length;
    return result;
}


byte
get_char(const TomlIterator &iterator)
{
    assert(!end_of_file(iterator));
    byte result = iterator.toml[iterator.position];
    return result;
}


bool
is_key_char(byte value)
{
    bool result =
        ((value >= 'a') && (value <= 'z'))
        || ((value >= 'A') && (value <= 'Z'))
        || ((value >= '0') && (value <= '9'))
        || (value == '-')
        || (value == '_');
    return result;
}


bool
match(const TomlIterator &iterator, byte value, string::size_type ahead = 0)
{
    assert(iterator.position <= iterator.length);

    bool result = false;
    if ((iterator.length - iterator.position) > ahead)
    {
        result = iterator.toml[iterator.position + ahead] == value;
    }

    return result;
}


bool
match_eol(const TomlIterator &iterator)
{
    bool result = false;

    if (end_of_file(iterator) || match(iterator, '\n'))
    {
        result = true;
    }
    else if (match(iterator, '\r') && match(iterator, '\n', 1))
    {
        result = true;
    }

    return result;
}


bool
match_whitespace(TomlIterator &iterator)
{
    bool result = match(iterator, ' ') || match(iterator, '\t');
    return result;
}


byte
eat_byte(TomlIterator &iterator, byte expected = INVALID_BYTE)
{
    assert(!end_of_file(iterator));

    byte result = iterator.toml[iterator.position];
    assert((expected == INVALID_BYTE) || (result == expected));
    ++iterator.position;

    if (result == '\n')
    {
        ++iterator.current_line;
        iterator.current_column = 1;
    }
    else
    {
        ++iterator.current_column;
    }

    return result;
}


void
eat_bytes(TomlIterator &iterator, string::size_type count)
{
    assert(count);
    for (string::size_type i = 0; i < count; ++i)
    {
        eat_byte(iterator);
    }
}


void
eat_comment(TomlIterator &iterator)
{
    if (match(iterator, '#'))
    {
        do
        {
            eat_byte(iterator);
        } while (!match_eol(iterator));
    }

    advance(iterator);
}


bool
eat_newline(TomlIterator &iterator)
{
    bool result = false;

    if (match(iterator, '\n'))
    {
        eat_byte(iterator);
        advance(iterator);
        result = true;
    }
    else if (match(iterator, '\r') && match(iterator, '\n', 1))
    {
        eat_bytes(iterator, 2);
        advance(iterator);
        result = true;
    }


    return result;
}


void
eat_whitespace(TomlIterator &iterator)
{
    while (match_whitespace(iterator))
    {
        eat_byte(iterator);
    }
    advance(iterator);
}


void
lex_string_char(TomlIterator &iterator, string &result)
{
    byte c = eat_byte(iterator);
    if (!((c == 0x09) || ((c >= 0x20) && (c <= 0x7e))))
    {
        fprintf(stderr, "Invalid or unhandled string char: '%c'\n", c);
        assert(false);
    }

    result.push_back(c);
}


void
lex_escape(TomlIterator &iterator, string &)
{
    eat_byte(iterator, '\\');

    fputs("Implement lex_escape\n", stderr);
    assert(false);
}


string
lex_string(TomlIterator &iterator, byte delimiter)
{
    assert((delimiter == '"') || (delimiter == '\''));
    eat_byte(iterator, delimiter);

    string result;
    bool lexing = true;
    while (lexing)
    {
        if (end_of_file(iterator))
        {
            assert(false);
            lexing = false;
        }
        else
        {
            byte c = get_char(iterator);
            if (c == delimiter)
            {
                eat_byte(iterator);
                lexing = false;
            }
            else if ((c == '\\') && (delimiter == '"'))
            {
                lex_escape(iterator, result);
            }
            else
            {
                lex_string_char(iterator, result);
            }
        }
    }

    return result;
}


void
lex_value(TomlIterator &iterator)
{
    assert(!match_eol(iterator));

    byte c = get_char(iterator);
    if ((c == '"') || (c == '\''))
    {
        string key = lex_string(iterator, c);
        add_token(iterator, TOKEN_STRING, move(key));
    }
    else
    {
        fprintf(stderr, "Invalid or unhandled value character: '%c'\n", c);
        assert(false);
    }
}


void
lex_unquoted_key(TomlIterator &iterator)
{
    string value;
    bool lexing = true;
    while (lexing)
    {
        if (end_of_file(iterator) || !is_key_char(get_char(iterator)))
        {
            lexing = false;
        }
        else
        {
            value.push_back(eat_byte(iterator));
        }
    }

    assert(value.length());
    add_token(iterator, TOKEN_KEY, move(value));
}


void
lex_simple_key(TomlIterator &iterator)
{
    assert(!match_whitespace(iterator) && !match_eol(iterator));

    byte c = get_char(iterator);
    if ((c == '"') || (c == '\''))
    {
        string key = lex_string(iterator, c);
        add_token(iterator, TOKEN_KEY, move(key));
    }
    else
    {
        lex_unquoted_key(iterator);
    }
}


void
lex_key(TomlIterator &iterator)
{
    bool lexing = true;
    while (lexing)
    {
        lex_simple_key(iterator);

        eat_whitespace(iterator);
        if (match(iterator, '.'))
        {
            eat_byte(iterator);
        }
        else
        {
            lexing = false;
        }
    }
}


void
lex_keyval(TomlIterator &iterator)
{
    lex_key(iterator);

    eat_whitespace(iterator);
    eat_byte(iterator, '=');
    eat_whitespace(iterator);

    lex_value(iterator);
}


void
lex_table(TomlIterator &)
{
    fputs("Implement lex_table", stderr);
    assert(false);
}


void
lex_expression(TomlIterator &iterator)
{
    eat_whitespace(iterator);

    if (match(iterator, '['))
    {
        lex_table(iterator);
    }
    else if (!match_eol(iterator) && !match(iterator, '#'))
    {
        lex_keyval(iterator);
    }

    eat_whitespace(iterator);
    eat_comment(iterator);
}


} // namespace


bool
lex_toml(const string &toml, TokenList &tokens)
{
    TomlIterator iterator = { toml, 0, toml.length(), 1, 1, 1, 1, tokens };

    while (!end_of_file(iterator))
    {
        bool lexing = true;
        while (lexing)
        {
            lex_expression(iterator);
            lexing = eat_newline(iterator);
        }

        assert(end_of_file(iterator));
    }

    return true;
}


} // namespace toml
