//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include "lexer.hpp"

#include <cstdio> // fputs
#include <iomanip>
#include <sstream>
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

    vector<Token> &tokens;
    vector<Error> &errors;
};


void
advance(TomlIterator &iterator)
{
    iterator.start_line = iterator.current_line;
    iterator.start_column = iterator.current_column;
}


void
add_error(TomlIterator &iterator, string message)
{
    Error error = { iterator.start_line, iterator.start_column, move(message) };
    iterator.errors.push_back(move(error));
    advance(iterator);
}


void
add_token(TomlIterator &iterator, TokenType type, string lexeme)
{
    Token token = { type, move(lexeme), iterator.start_line, iterator.start_column };
    iterator.tokens.push_back(move(token));
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
resynchronize(TomlIterator &iterator, string message)
{
    bool eating = true;
    while (eating)
    {
        if (match_eol(iterator))
        {
            eating = false;
        }
        else
        {
            message.push_back(eat_byte(iterator));
        }
    }

    add_error(iterator, move(message));
}


constexpr byte B10000000 = 0x80;
constexpr byte B11000000 = 0xc0;
constexpr byte B11100000 = 0xe0;
constexpr byte B11110000 = 0xf0;
constexpr byte B11111000 = 0xf8;

constexpr byte B00000111 = 0x07;
constexpr byte B00001111 = 0x0f;
constexpr byte B00011111 = 0x1f;
constexpr byte B00111111 = 0x3f;
constexpr byte B01111111 = 0x7f;


void
byte_to_hex(ostream &o, u8 value)
{
    o << setw(2) << setfill('0') << uppercase << hex << static_cast<u32>(value);
}


void
format_unicode(ostream &o, u32 codepoint)
{
    o << "U+" << setw(4) << setfill('0') << uppercase << hex << codepoint;
}


void
lex_string_char(TomlIterator &iterator, string &result)
{
    assert(!end_of_file(iterator));

    byte c = eat_byte(iterator);
    result.push_back(c);

    u32 codepoint = 0;
    u32 nbytes = 0;
    bool valid = true;

    if ((c & B10000000) == 0)
    {
        codepoint = c;
        nbytes = 1;
    }
    else if ((c & B11100000) == B11000000)
    {
        codepoint = ((c & B00011111) << 6);
        nbytes = 2;
    }
    else if ((c & B11110000) == B11100000)
    {
        codepoint = ((c & B00001111) << 12);
        nbytes = 3;
    }
    else if ((c & B11111000) == B11110000)
    {
        codepoint = ((c & B00001111) << 18);
        nbytes = 4;
    }
    else
    {
        ostringstream message;
        message << "Invalid UTF-8 byte: 0x";
        byte_to_hex(message, c);
        add_error(iterator, move(message.str()));
        nbytes = 1; // Let's just eat the byte and (try to) keep going
        valid = false;
    }

    u32 eaten = 1;
    for ( ; (eaten < nbytes) && !end_of_file(iterator); ++eaten)
    {
        c = eat_byte(iterator);
        result.push_back(c);

        if ((c & B11000000) == B10000000)
        {
            byte masked = c & B00111111;
            u32 shift = 6 * (nbytes - 1 - eaten);
            codepoint |= (masked << shift);
        }
        else
        {
            ostringstream message;
            message << "Invalid UTF-8 byte: 0x";
            byte_to_hex(message, c);
            message << ". Expected a continuation byte in the range of 0x";
            byte_to_hex(message, 0x80);
            message << "-";
            byte_to_hex(message, 0xbf);
            add_error(iterator, move(message.str()));
            valid = false;
        }
    }

    // if eaten < nbytes, then we'll err on an unterminated string
    if (valid && (eaten == nbytes))
    {
        if ((nbytes == 2) && (codepoint < 0x80))
        {
            ostringstream message;
            message << "Overlong encoding of Unicode value: ";
            format_unicode(message, codepoint);
            message << " is a 1-byte value but was encoded in 2 bytes.";
            add_error(iterator, move(message.str()));
        }
        else if ((nbytes == 3) && (codepoint < 0x800))
        {
            u32 expected_bytes = (codepoint < 0x80) ? 1 : 2;

            ostringstream message;
            message << "Overlong encoding of Unicode value: ";
            format_unicode(message, codepoint);
            message << " is a " << expected_bytes << "-byte value but was encoded in 3 bytes.";
            add_error(iterator, move(message.str()));
        }
        else if ((nbytes == 4) && (codepoint < 0x10000))
        {
            u32 expected_bytes = 1;
            if (codepoint < 0x800)
            {
                expected_bytes = 3;
            }
            else if (codepoint < 0x80)
            {
                expected_bytes = 2;
            }

            ostringstream message;
            message << "Overlong encoding of Unicode value: ";
            format_unicode(message, codepoint);
            message << " is a " << expected_bytes << "-byte value but was encoded in 4 bytes.";
            add_error(iterator, move(message.str()));
        }
        else if (codepoint > 0x10ffff)
        {
            ostringstream message;
            message << "Invalid Unicode value: ";
            format_unicode(message, codepoint);
            message << " (maximum allowed value is ";
            format_unicode(message, 0x10ffff);
            message << ")";
            add_error(iterator, move(message.str()));
        }
        else if (!(
            (codepoint == 0x09)
            || ((codepoint >= 0x20) && (codepoint <= 0x7e))
            || ((codepoint >= 0x80) && (codepoint <= 0xd7ff))
            || (codepoint >= 0xe000)))
        {
            ostringstream message;
            message << "Unicode value ";
            format_unicode(message, codepoint);
            message << " is not allowed in a string";
            add_error(iterator, move(message.str()));
        }

        // Adjust the current column to reflect only 1 "character". Although
        // this probably falls apart with combining characters, etc. Pull
        // requests accepted. :-)
        iterator.current_column -= (nbytes - 1);
    }
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
    else if (match(iterator, '#'))
    {
        add_error(iterator, "Missing value");
    }
    else
    {
        resynchronize(iterator, "Invalid value: ");
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

    if (!value.length())
    {
        add_error(iterator, "Missing key");
    }

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
            eat_whitespace(iterator);
            advance(iterator);
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
lex_toml(const string &toml, vector<Token> &tokens, vector<Error> &errors)
{
    TomlIterator iterator = { toml, 0, toml.length(), 1, 1, 1, 1, tokens, errors };

    while (!end_of_file(iterator))
    {
        bool lexing = true;
        while (lexing)
        {
            lex_expression(iterator);
            lexing = eat_newline(iterator);
        }

        if (!end_of_file(iterator))
        {
            resynchronize(iterator, "Expected new line but got: ");
        }
    }

    return errors.size() == 0;
}


} // namespace toml
