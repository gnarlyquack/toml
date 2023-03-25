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


using IsDigit = bool (*)(byte value);


// No UTF-8 byte can be 0xff
constexpr byte INVALID_BYTE = BYTE_MAX;


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

constexpr char HEX2CHAR[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                              'A', 'B', 'C', 'D', 'E', 'F' };


enum LexingContext
{
    LEX_EOL         =      0,
    LEX_FRACTION    = 1 << 0,
    LEX_EXPONENT    = 1 << 1,
    LEX_DATE        = 1 << 2,
    LEX_TIME        = 1 << 3,
    LEX_DATETIME    = 1 << 4,
    LEX_TIMEZONE    = 1 << 5,
    LEX_ARRAY       = 1 << 6,
    LEX_TABLE       = 1 << 7,
};


enum ContainerState
{
    CONTAINER_START,
    CONTAINER_VALUE,
    CONTAINER_SEPARATOR,
};


struct TomlIterator
{
    const string &toml;
    string::size_type length;

    string::size_type start_position;
    string::size_type start_line;
    string::size_type start_column;

    string::size_type current_position;
    string::size_type current_line;
    string::size_type current_column;

    vector<Token> &tokens;
    vector<Error> &errors;
};


struct LexDigitResult
{
    static constexpr u32 INVALID_DIGIT         = 1 << 0;
    static constexpr u32 HAS_UNDERSCORE        = 1 << 1;
    static constexpr u32 INVALID_UNDERSCORE    = 1 << 2;

    string digits;
    u32 flags;
};


//
// Predeclarations
//

void
lex_keyval(TomlIterator &iterator, u32 context);

void
lex_value(TomlIterator &iterator, u32 context);


//
// Implementation
//

void
byte_to_hex(byte value, string &out)
{
    out += "0x";
    out.push_back(HEX2CHAR[(value >> 4) & 0xf]);
    out.push_back(HEX2CHAR[value & 0xf]);
}


void
advance(TomlIterator &iterator)
{
    iterator.start_position = iterator.current_position;
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
add_token(TomlIterator &iterator, TokenType type, string lexeme = "")
{
    Token token = { type, move(lexeme), iterator.start_line, iterator.start_column };
    iterator.tokens.push_back(move(token));
    advance(iterator);
}


bool
prev_token(const vector<Token> &tokens, TokenType type)
{
    bool result = tokens.size() && (tokens.back().type == type);
    return result;
}


bool
end_of_file(const TomlIterator &iterator, u64 ahead = 0)
{
    assert(iterator.current_position <= iterator.length);
    bool result = iterator.length - iterator.current_position <= ahead;
    return result;
}


byte
get_byte(const TomlIterator &iterator, u64 ahead = 0)
{
    byte result = INVALID_BYTE;
    if (!end_of_file(iterator, ahead))
    {
        result = iterator.toml[iterator.current_position + ahead];
    }
    return result;
}


string
get_lexeme(const TomlIterator &iterator)
{
    string result = iterator.toml.substr(
        iterator.start_position,
        iterator.current_position - iterator.start_position + 1);
    return result;
}


bool
is_binary(byte value)
{
    bool result = (value == '0') || (value <= '1');
    return result;
}


bool
is_decimal(byte value)
{
    bool result = (value >= '0') && (value <= '9');
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
is_hexadecimal(byte value)
{
    bool result =
        ((value >= '0') && (value <= '9'))
        || ((value >= 'a') && (value <= 'f'))
        || ((value >= 'A') && (value <= 'F'));
    return result;
}


bool
is_octal(byte value)
{
    bool result = (value >= '0') && (value <= '7');
    return result;
}


bool
is_printable(byte value)
{
    bool result = (value >= 0x20) && (value <= 0x7e); // ASCII printable range
    return result;
}


bool
match(const TomlIterator &iterator, byte value, string::size_type ahead = 0)
{
    assert(iterator.current_position <= iterator.length);

    bool result = false;
    if ((iterator.length - iterator.current_position) > ahead)
    {
        result = iterator.toml[iterator.current_position + ahead] == value;
    }

    return result;
}


bool
match_eol(const TomlIterator &iterator, u32 ahead = 0)
{
    bool result = false;

    if (end_of_file(iterator, ahead) || match(iterator, '\n', ahead))
    {
        result = true;
    }
    else if (match(iterator, '\r', ahead) && match(iterator, '\n', ahead + 1))
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

    byte result = iterator.toml[iterator.current_position];
    assert((expected == INVALID_BYTE) || (result == expected));

    ++iterator.current_position;
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
eat_bytes(TomlIterator &iterator, string::size_type count, byte expected = INVALID_BYTE)
{
    assert(count);
    for (string::size_type i = 0; i < count; ++i)
    {
        eat_byte(iterator, expected);
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
}


bool
eat_newline(TomlIterator &iterator)
{
    bool result = false;

    if (match(iterator, '\n'))
    {
        eat_byte(iterator);
        result = true;
    }
    else if (match(iterator, '\r') && match(iterator, '\n', 1))
    {
        eat_bytes(iterator, 2);
        result = true;
    }

    return result;
}


bool
eat_string(TomlIterator &iterator, const string &expected)
{
    bool result = false;

    string eaten;
    while (!match_eol(iterator) && !match_whitespace(iterator))
    {
        eaten.push_back(eat_byte(iterator));
    }

    if (eaten == expected)
    {
        result = true;
    }
    else
    {
        add_error(iterator, "Invalid value: " + eaten);
    }

    return result;
}


bool
eat_whitespace(TomlIterator &iterator)
{
    bool result = false;

    if (match_whitespace(iterator))
    {
        result = true;
        do
        {
            eat_byte(iterator);
        } while (match_whitespace(iterator));
    }

    return result;
}


void
convert_unicode_to_utf8(u32 codepoint, string &out)
{
    assert(codepoint <= 0x10ffff);

    u32 nbytes = 1;
    if (codepoint < 0x80)
    {
        out.push_back(codepoint & B01111111);
    }
    else if (codepoint <= 0x800)
    {
        nbytes = 2;
        byte c = B11000000 | ((codepoint >> 6) & B00011111);
        out.push_back(c);
    }
    else if (codepoint <= 0x10000)
    {
        nbytes = 3;
        byte c = B11100000 | ((codepoint >> 12) & B00001111);
        out.push_back(c);
    }
    else
    {
        nbytes = 4;
        byte c = B11110000 | ((codepoint >> 18) & B00000111);
        out.push_back(c);
    }

    for (u32 i = 1; i < nbytes; ++i)
    {
        u32 shift = 6 * (nbytes - 1 - i);
        byte c = B10000000 | ((codepoint >> shift) & B00111111);
        out.push_back(c);
    }
}


void
format_unicode(ostream &o, u32 codepoint)
{
    o << "U+" << setw(4) << setfill('0') << uppercase << hex << codepoint;
}


void
resynchronize(TomlIterator &iterator, string message)
{
    advance(iterator);
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


void
invalid_escape(TomlIterator &iterator, byte value)
{
    string message = "Invalid escape: ";
    if (is_printable(value))
    {
        message.push_back(value);
    }
    else
    {
        byte_to_hex(value, message);
    }

    Error error = { iterator.current_line, iterator.current_column, move(message) };
    iterator.errors.push_back(move(error));

    eat_byte(iterator);
}


void
invalid_unicode(TomlIterator &iterator, u32 codepoint)
{
    string message = "Invalid Unicode character: U+";

    u32 nchars = (codepoint / 0xf) + 1;
    for (u32 i = 4; i > nchars; --i)
    {
        message.push_back('0');
    }

    for( ; nchars; --nchars)
    {
        u32 shift = 4 * (nchars - 1);
        message.push_back(HEX2CHAR[(codepoint >> shift) & 0xf]);
    }

    Error error = { iterator.start_line, iterator.start_column, move(message) };
    iterator.errors.push_back(move(error));
}


void
invalid_unicode_escape(TomlIterator &iterator, u64 position)
{
    string message = "Unicode escape sequence contains non-hexadecimal value: ";
    byte value = get_byte(iterator, position);

    if (is_printable(value))
    {
        message.push_back(value);
    }
    else
    {
        byte_to_hex(value, message);
    }


    Error error = { iterator.start_line, iterator.start_column, move(message) };
    iterator.errors.push_back(move(error));
}


void
validate_digits(TomlIterator &iterator, const LexDigitResult &result, const string &type, bool no_sign = false)
{
    if (no_sign && (prev_token(iterator.tokens, TOKEN_MINUS) || prev_token(iterator.tokens, TOKEN_PLUS)))
    {
        const Token &token = iterator.tokens.back();
        Error error = { token.line, token.column, "'" + token.lexeme + "' sign not allowed for " + type + "number" };
        iterator.errors.push_back(move(error));
    }

    if (result.flags & result.INVALID_DIGIT)
    {
        add_error(iterator, "Invalid " + type + " number: " + get_lexeme(iterator));
    }
    else if (result.flags & result.INVALID_UNDERSCORE)
    {
        add_error(iterator, "'_' must separate digits in " + type + " number: " + get_lexeme(iterator));
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing " + type + " number");
    }
}


void
validate_hour(TomlIterator &iterator, const LexDigitResult &result, bool no_sign = true)
{
    if (no_sign && (prev_token(iterator.tokens, TOKEN_MINUS) || prev_token(iterator.tokens, TOKEN_PLUS)))
    {
        const Token &token = iterator.tokens.back();
        Error error = { token.line, token.column, "'" + token.lexeme + "' sign not allowed for time" };
        iterator.errors.push_back(move(error));
    }

    if (result.flags)
    {
        add_error(iterator, "Invalid hour: " + get_lexeme(iterator));
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing hour");
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Hour must be two digits");
    }
    else if ((result.digits[0] > '2') || ((result.digits[0] == '2') && (result.digits[2] > '3')))
    {
        add_error(iterator, "Hour must be between 00 and 23 inclusive");
    }
}


void
validate_minute(TomlIterator &iterator, const LexDigitResult &result)
{
    if (result.flags)
    {
        add_error(iterator, "Invalid minute: " + get_lexeme(iterator));
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing minute");
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Minute must be two digits");
    }
    else if (result.digits[0] > '5')
    {
        add_error(iterator, "Minute must be between 00 and 59 inclusive");
    }
}


void
validate_second(TomlIterator &iterator, const LexDigitResult &result)
{
    if (result.flags)
    {
        add_error(iterator, "Invalid second: " + get_lexeme(iterator));
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing second");
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Second must be two digits");
    }
    else if (result.digits[0] > '5')
    {
        // TODO support leap seconds?
        add_error(iterator, "Second must be between 00 and 59 inclusive");
    }
}


void
validate_year(TomlIterator &iterator, const LexDigitResult &result)
{
    if (prev_token(iterator.tokens, TOKEN_MINUS) || prev_token(iterator.tokens, TOKEN_PLUS))
    {
        const Token &token = iterator.tokens.back();
        Error error = { token.line, token.column, "'" + token.lexeme + "' sign not allowed for date" };
        iterator.errors.push_back(move(error));
    }

    if (result.flags)
    {
        add_error(iterator, "Invalid year: " + get_lexeme(iterator));
    }
    else if (result.digits.length() > 4)
    {
        add_error(iterator, "Year must be four digits");
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing year");
    }
}


void
validate_month(TomlIterator &iterator, const LexDigitResult &result)
{
    if (result.flags)
    {
        add_error(iterator, "Invalid month: " + get_lexeme(iterator));
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Month must be two digits");
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing month");
    }
    else if (((result.digits[0] == '0') && (result.digits[1] == '0'))
            || ((result.digits[0] == '1') && (result.digits[1] > '2'))
            || (result.digits[0] > '1'))
    {
        add_error(iterator, "Month must be between 01 and 12 inclusive");
    }
}


void
validate_day(TomlIterator &iterator, const LexDigitResult &result)
{
    if (result.flags)
    {
        add_error(iterator, "Invalid day: " + get_lexeme(iterator));
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Day must be two digits");
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing day");
    }
    else if (((result.digits[0] == '0') && (result.digits[0] == '0'))
            || ((result.digits[0] == '3') && (result.digits[1] > '1'))
            || (result.digits[0] > '3'))
    {
        // TODO Validate day based on month and year while lexing?
        add_error(iterator, "Day must be between 01 and 31 inclusive");
    }
}


LexDigitResult
lex_digits(TomlIterator &iterator, IsDigit is_digit, u32 context)
{
    LexDigitResult result = {};
    bool underscore_allowed = false;
    bool lexing = true;
    while (lexing && !match_eol(iterator) && !match_whitespace(iterator))
    {
        byte c = get_byte(iterator);
        if (is_digit(c))
        {
            eat_byte(iterator);
            result.digits.push_back(c);
            underscore_allowed = true;
        }
        else if (c == '_')
        {
            eat_byte(iterator);
            result.flags |= result.HAS_UNDERSCORE;
            if (underscore_allowed)
            {
                underscore_allowed = false;
            }
            else
            {
                result.flags |= result.INVALID_UNDERSCORE;
            }
        }
        else if (
            ((c == '.') && (context & LEX_FRACTION))
            || (((c == 'e') || (c == 'E')) && (context & LEX_EXPONENT))
            || ((c == '-') && ((context & LEX_DATE) || (context & LEX_TIMEZONE)))
            || ((c == ':') && (context & LEX_TIME))
            || (((c == 'T') || (c == 't')) && (context & LEX_DATETIME))
            || (((c == '+') || (c == 'Z') || (c == 'z')) && (context & LEX_TIMEZONE))
            || ((c == ',') && ((context & LEX_ARRAY) || (context & LEX_TABLE)))
            || ((c == ']') && (context & LEX_ARRAY))
            || ((c == '}') && (context & LEX_TABLE)))
        {
            lexing = false;
        }
        else
        {
            eat_byte(iterator);
            result.flags |= result.INVALID_DIGIT;
        }
    }

    if ((result.flags & result.HAS_UNDERSCORE) && !underscore_allowed)
    {
        result.flags |= result.INVALID_UNDERSCORE;
    }

    return result;
}


void
lex_minus(TomlIterator &iterator)
{
    eat_byte(iterator, '-');
    add_token(iterator, TOKEN_MINUS, "-");
}


void
lex_plus(TomlIterator &iterator)
{
    eat_byte(iterator, '+');
    add_token(iterator, TOKEN_PLUS, "+");
}


void
lex_exponent(TomlIterator &iterator, u32 context)
{
    advance(iterator);

    if (match(iterator, '-'))
    {
        lex_minus(iterator);
    }
    else if (match(iterator, '+'))
    {
        lex_plus(iterator);
    }

    LexDigitResult exponent = lex_digits(iterator, is_decimal, context);
    validate_digits(iterator, exponent, "exponential part of decimal");
    add_token(iterator, TOKEN_EXPONENT, exponent.digits);
}


void
lex_fraction(TomlIterator &iterator, u32 context)
{
    advance(iterator);
    LexDigitResult fraction = lex_digits(iterator, is_decimal, context | LEX_EXPONENT);
    validate_digits(iterator, fraction, "fractional part of decimal");
    add_token(iterator, TOKEN_FRACTION, fraction.digits);

    if (match(iterator, 'e') || match(iterator, 'E'))
    {
        eat_byte(iterator);
        lex_exponent(iterator, context);
    }
}


void
lex_time(TomlIterator &iterator, u32 context)
{
    advance(iterator);
    LexDigitResult minute = lex_digits(iterator, is_decimal, context | LEX_TIME | LEX_FRACTION);
    validate_minute(iterator, minute);
    add_token(iterator, TOKEN_MINUTE, move(minute.digits));

    eat_byte(iterator, ':');
    advance(iterator);
    LexDigitResult second = lex_digits(iterator, is_decimal, context | LEX_FRACTION);
    validate_second(iterator, second);
    add_token(iterator, TOKEN_SECOND, move(second.digits));

    if (match(iterator, '.'))
    {
        eat_byte(iterator);
        advance(iterator);
        LexDigitResult fraction = lex_digits(iterator, is_decimal, context);
        if (fraction.flags)
        {
            add_error(iterator, "Invalid value for fraction seconds: " + get_lexeme(iterator));
        }
        else if (fraction.digits.length() == 0)
        {
            add_error(iterator, "Missing fractional seconds");
        }
        add_token(iterator, TOKEN_FRACTION, fraction.digits);
    }
}


void
lex_hour(TomlIterator &iterator, u32 context)
{
    advance(iterator);
    LexDigitResult result = lex_digits(iterator, is_decimal, context | LEX_TIME | LEX_FRACTION);
    validate_hour(iterator, result);
    add_token(iterator, TOKEN_HOUR, move(result.digits));

    eat_byte(iterator, ':');
    lex_time(iterator, context);
}


void
lex_timezone(TomlIterator &iterator, u32 context)
{
    LexDigitResult hour = lex_digits(iterator, is_decimal, context | LEX_TIME);
    validate_hour(iterator, hour, false);
    add_token(iterator, TOKEN_HOUR, hour.digits);

    eat_byte(iterator, ':');
    advance(iterator);

    LexDigitResult minute = lex_digits(iterator, is_decimal, context);
    validate_hour(iterator, minute);
    add_token(iterator, TOKEN_MINUTE, minute.digits);
}


void
lex_date(TomlIterator &iterator, u32 context)
{
    advance(iterator);
    LexDigitResult month = lex_digits(iterator, is_decimal, context | LEX_DATE | LEX_DATETIME);
    validate_month(iterator, month);
    add_token(iterator, TOKEN_MONTH, month.digits);

    eat_byte(iterator, '-');
    advance(iterator);
    LexDigitResult day = lex_digits(iterator, is_decimal, context | LEX_DATETIME);
    validate_day(iterator, day);
    add_token(iterator, TOKEN_DAY, day.digits);

    if (match(iterator, 'T') || match(iterator, 't')
        || (match(iterator, ' ') && !match_eol(iterator, 1) && is_decimal(get_byte(iterator, 1))))
    {
        eat_byte(iterator);
        lex_hour(iterator, LEX_TIMEZONE);

        if (match(iterator, 'Z') ||match(iterator, 'z'))
        {
            eat_byte(iterator);
            add_token(iterator, TOKEN_PLUS, "+");
            add_token(iterator, TOKEN_HOUR, "00");
            add_token(iterator, TOKEN_MINUTE, "00");
        }
        else if (match(iterator, '+'))
        {
            lex_plus(iterator);
            lex_timezone(iterator, context);
        }
        else if (match(iterator, '-'))
        {
            lex_minus(iterator);
            lex_timezone(iterator, context);
        }
    }
}


void
lex_decimal(TomlIterator &iterator, u32 context)
{
    LexDigitResult result = lex_digits(iterator, is_decimal, context | LEX_FRACTION | LEX_EXPONENT | LEX_DATE | LEX_TIME);

    if (match(iterator, '.'))
    {
        validate_digits(iterator, result, "whole part of decimal");
        add_token(iterator, TOKEN_DECIMAL, move(result.digits));
        eat_byte(iterator);
        lex_fraction(iterator, context);
    }
    else if (match(iterator, 'e') || match(iterator, 'E'))
    {
        validate_digits(iterator, result, "whole part of decimal");
        add_token(iterator, TOKEN_DECIMAL, move(result.digits));
        eat_byte(iterator);
        lex_exponent(iterator, context);
    }
    else if (match(iterator, '-'))
    {
        validate_year(iterator, result);
        add_token(iterator, TOKEN_YEAR, result.digits);
        eat_byte(iterator);
        lex_date(iterator, context);
    }
    else if (match(iterator, ':'))
    {
        validate_hour(iterator, result);
        add_token(iterator, TOKEN_HOUR, move(result.digits));
        eat_byte(iterator);
        lex_time(iterator, context);
    }
    else
    {
        validate_digits(iterator, result, "decimal");
        add_token(iterator, TOKEN_DECIMAL, move(result.digits));
    }
}


void
lex_hexadecimal(TomlIterator &iterator, u32 context)
{
    LexDigitResult result = lex_digits(iterator, is_hexadecimal, context);
    validate_digits(iterator, result, "hexadecimal", true);
    add_token(iterator, TOKEN_HEXADECIMAL, move(result.digits));
}


void
lex_octal(TomlIterator &iterator, u32 context)
{
    LexDigitResult result = lex_digits(iterator, is_octal, context);
    validate_digits(iterator, result, "octal", true);
    add_token(iterator, TOKEN_OCTAL, move(result.digits));
}


void
lex_binary(TomlIterator &iterator, u32 context)
{
    LexDigitResult result = lex_digits(iterator, is_binary, context);
    validate_digits(iterator, result, "binary", true);
    add_token(iterator, TOKEN_BINARY, move(result.digits));
}


void
lex_number(TomlIterator &iterator, u32 context)
{
    if (match(iterator, '0'))
    {
        if (match(iterator, 'x', 1))
        {
            eat_bytes(iterator, 2);
            lex_hexadecimal(iterator, context);
        }
        else if (match(iterator, 'o', 1))
        {
            eat_bytes(iterator, 2);
            lex_octal(iterator, context);
        }
        else if (match(iterator, 'b', 1))
        {
            eat_bytes(iterator, 2);
            lex_binary(iterator, context);
        }
        else
        {
            lex_decimal(iterator, context);
        }
    }
    else
    {
        lex_decimal(iterator, context);
    }
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
        string message = "Invalid UTF-8 byte: ";
        byte_to_hex(c, message);
        add_error(iterator, move(message));
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
            string message = "Invalid UTF-8 byte: ";
            byte_to_hex(c, message);
            message += ". Expected a continuation byte in the range of ";
            byte_to_hex(0x80, message);
            message += "-";
            byte_to_hex(0xbf, message);
            add_error(iterator, move(message));
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
lex_unicode(TomlIterator &iterator, string &result, u64 count)
{
    assert((count == 4) || (count == 8));

    u32 codepoint = 0;
    bool valid = true;
    u64 lexed = 0;
    for ( ; (lexed < count) && !end_of_file(iterator, lexed); ++lexed)
    {
        byte c = get_byte(iterator, lexed);

        if ((c >= '0') && (c <= '9'))
        {
            c -= '0';
        }
        else if ((c >= 'a') && (c <= 'f'))
        {
            // In Unicode/ASCII: 'a' == 97 -> 10 == 'a' - 87
            c -= 87;
        }
        else if ((c >= 'A') && (c <= 'F'))
        {
            // In Unicode/ASCII: 'A' == 65 -> 10 == 'A' - 55
            c -= 55;
        }
        else
        {
            valid = false;
            break;
        }

        u64 shift = 4 * (count - 1 - lexed);
        codepoint |= (c << shift);
    }

    if (valid && (lexed == count))
    {
        if ((codepoint <= 0xd7ff) || ((codepoint >= 0xe000) && (codepoint <= 0x10ffff)))
        {
            convert_unicode_to_utf8(codepoint, result);
        }
        else
        {
            invalid_unicode(iterator, codepoint);
        }
    }
    else if (!valid)
    {
        invalid_unicode_escape(iterator, lexed);
    }

    eat_bytes(iterator, lexed);
}


void
lex_escape(TomlIterator &iterator, string &result)
{
    assert (!end_of_file(iterator));

    byte c = get_byte(iterator);
    switch (get_byte(iterator))
    {
        case '"':
        case '\'':
        case '\\':
        {
            eat_byte(iterator);
            result.push_back(c);
        } break;

        case 'b':
        {
            eat_byte(iterator);
            result.push_back('\b');
        } break;

        case 'f':
        {
            eat_byte(iterator);
            result.push_back('\f');
        } break;

        case 'n':
        {
            eat_byte(iterator);
            result.push_back('\n');
        } break;

        case 'r':
        {
            eat_byte(iterator);
            result.push_back('\r');
        } break;

        case 't':
        {
            eat_byte(iterator);
            result.push_back('\t');
        } break;

        case 'u':
        {
            eat_byte(iterator);
            lex_unicode(iterator, result, 4);
        } break;

        case 'U':
        {
            eat_byte(iterator);
            lex_unicode(iterator, result, 8);
        } break;

        default:
        {
            invalid_escape(iterator, c);
            eat_byte(iterator);
            result.push_back(c);
        }
    }
}


string
lex_multiline_string(TomlIterator &iterator, byte delimiter)
{
    assert((delimiter == '"') || (delimiter == '\''));
    eat_bytes(iterator, 3, delimiter);
    eat_newline(iterator);

    string result;
    bool lexing = true;
    while (lexing)
    {
        if (end_of_file(iterator))
        {
            assert(false);
            lexing = false;
        }
        else if (eat_newline(iterator))
        {
            result.push_back('\n');
        }
        else
        {
            byte c = get_byte(iterator);
            if (c == delimiter)
            {
                u32 i = 1;
                for ( ; match(iterator, delimiter, i) && (i < 5); ++i);

                eat_bytes(iterator, i);
                switch (i)
                {
                    case 1:
                    {
                        result.push_back(delimiter);
                    } break;

                    case 2:
                    {
                        result.push_back(delimiter);
                        result.push_back(delimiter);
                    } break;

                    case 3:
                    {
                        lexing = false;
                    } break;

                    case 4:
                    {
                        result.push_back(delimiter);
                        lexing = false;
                    } break;

                    case 5:
                    {
                        result.push_back(delimiter);
                        result.push_back(delimiter);
                        lexing = false;
                    } break;
                }
            }
            else if ((c == '\\') && (delimiter == '"'))
            {
                eat_byte(iterator);
                if (!end_of_file(iterator))
                {
                    c = get_byte(iterator);
                    if (eat_whitespace(iterator) || match_eol(iterator))
                    {
                        bool eating = eat_newline(iterator);
                        if (eating)
                        {
                            while (eating)
                            {
                                eat_whitespace(iterator);
                                eating = eat_newline(iterator);
                            }
                        }
                        else
                        {
                            invalid_escape(iterator, c);
                        }
                    }
                    else
                    {
                        lex_escape(iterator, result);
                    }
                }
            }
            else
            {
                lex_string_char(iterator, result);
            }
        }
    }

    return result;
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
            byte c = get_byte(iterator);
            if (c == delimiter)
            {
                eat_byte(iterator);
                lexing = false;
            }
            else if ((c == '\\') && (delimiter == '"'))
            {
                eat_byte(iterator);
                if (!end_of_file(iterator))
                {
                    lex_escape(iterator, result);
                }
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
lex_array(TomlIterator &iterator)
{
    eat_byte(iterator, '[');
    add_token(iterator, TOKEN_LBRACKET);

    ContainerState state = CONTAINER_START;
    bool lexing = true;
    while (lexing)
    {
        eat_whitespace(iterator);
        if (end_of_file(iterator))
        {
            add_error(iterator, "Unterminated array");
        }
        else if (match_eol(iterator))
        {
            eat_newline(iterator);
        }
        else
        {
            switch (get_byte(iterator))
            {
                case ']':
                {
                    advance(iterator);
                    eat_byte(iterator);
                    add_token(iterator, TOKEN_RBRACKET);
                    lexing = false;
                } break;

                case ',':
                {
                    if (state != CONTAINER_VALUE)
                    {
                        add_error(iterator, "Missing value for array");
                    }

                    advance(iterator);
                    eat_byte(iterator);
                    add_token(iterator, TOKEN_COMMA);
                    state = CONTAINER_SEPARATOR;
                } break;

                case '#':
                {
                    eat_comment(iterator);
                    eat_newline(iterator);
                } break;

                default:
                {
                    if (state == CONTAINER_VALUE)
                    {
                        add_error(iterator, "Missing ',' between array values");
                    }
                    lex_value(iterator, LEX_ARRAY);
                    state = CONTAINER_VALUE;
                }
            }
        }
    }
}


void
lex_inline_table(TomlIterator &iterator)
{
    eat_byte(iterator, '{');
    add_token(iterator, TOKEN_LBRACE);

    ContainerState state = CONTAINER_START;
    bool lexing = true;
    while (lexing)
    {
        eat_whitespace(iterator);
        switch (get_byte(iterator))
        {
            case '}':
            {
                advance(iterator);
                if (state == CONTAINER_SEPARATOR)
                {
                    add_error(iterator, "Trailing ',' is not allowed for an inline table");
                }
                eat_byte(iterator);
                add_token(iterator, TOKEN_RBRACE);
                lexing = false;
            } break;

            case ',':
            {
                advance(iterator);
                if (state != CONTAINER_VALUE)
                {
                    add_error(iterator, "Missing value for inline table");
                }
                eat_byte(iterator);
                add_token(iterator, TOKEN_COMMA);
                state = CONTAINER_SEPARATOR;
            } break;

            default:
            {
                if (match_eol(iterator))
                {
                    add_error(iterator, "Unterminated inline table");
                    lexing = false;
                }
                else
                {
                    if (state == CONTAINER_VALUE)
                    {
                        add_error(iterator, "Missing ',' between inline table values");
                    }
                    lex_keyval(iterator, LEX_TABLE);
                    state = CONTAINER_VALUE;
                }
            }
        }
    }
}


void
lex_value(TomlIterator &iterator, u32 context)
{
    assert(!match_whitespace(iterator) && !match_eol(iterator));
    advance(iterator);

    byte c = get_byte(iterator);

    if (is_decimal(c))
    {
        lex_number(iterator, context);
    }
    else
    {
        switch (c)
        {
            case '"':
            case '\'':
            {
                if (match(iterator, c, 1) && match(iterator, c, 2))
                {
                    string value = lex_multiline_string(iterator, c);
                    add_token(iterator, TOKEN_STRING, move(value));
                }
                else
                {
                    string value = lex_string(iterator, c);
                    add_token(iterator, TOKEN_STRING, move(value));
                }
            } break;

            case '-':
            {
                lex_minus(iterator);
                if (match(iterator, 'i'))
                {
                    eat_string(iterator, "inf");
                    add_token(iterator, TOKEN_INF);
                }
                else if (match(iterator, 'n'))
                {
                    eat_string(iterator, "nan");
                    add_token(iterator, TOKEN_NAN);
                }
                else
                {
                    lex_number(iterator, context);
                }
            } break;

            case '+':
            {
                lex_plus(iterator);
                if (match(iterator, 'i'))
                {
                    eat_string(iterator, "inf");
                    add_token(iterator, TOKEN_INF);
                }
                else if (match(iterator, 'n'))
                {
                    eat_string(iterator, "nan");
                    add_token(iterator, TOKEN_NAN);
                }
                else
                {
                    lex_number(iterator, context);
                }
            } break;

            case 'i':
            {
                eat_string(iterator, "inf");
                add_token(iterator, TOKEN_INF);
            } break;

            case 'n':
            {
                eat_string(iterator, "nan");
                add_token(iterator, TOKEN_NAN);
            } break;


            case 't':
            {
                eat_string(iterator, "true");
                add_token(iterator, TOKEN_TRUE);
            } break;

            case 'f':
            {
                eat_string(iterator, "false");
                add_token(iterator, TOKEN_FALSE);
            } break;

            case '[':
            {
                lex_array(iterator);
            } break;

            case '{':
            {
                lex_inline_table(iterator);
            } break;

            // special handling for invalid cases
            case '.':
            {
                lex_number(iterator, context);
            } break;

            case '#':
            {
                add_error(iterator, "Missing value");
                add_token(iterator, TOKEN_ERROR);
            } break;

            default:
            {
                resynchronize(iterator, "Invalid value: ");
            } break;
        }
    }
}


void
lex_unquoted_key(TomlIterator &iterator)
{
    string value;
    bool lexing = true;
    while (lexing)
    {
        if (end_of_file(iterator) || !is_key_char(get_byte(iterator)))
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

    byte c = get_byte(iterator);
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
        advance(iterator);
        lex_simple_key(iterator);

        eat_whitespace(iterator);
        if (match(iterator, '.'))
        {
            eat_byte(iterator);
            eat_whitespace(iterator);
        }
        else
        {
            lexing = false;
        }
    }
}


void
lex_keyval(TomlIterator &iterator, u32 context)
{
    lex_key(iterator);

    eat_whitespace(iterator);
    eat_byte(iterator, '=');
    eat_whitespace(iterator);

    lex_value(iterator, context);
}


void
lex_table(TomlIterator &iterator)
{
    advance(iterator);

    eat_byte(iterator, '[');
    bool table_array = false;

    if (match(iterator, '['))
    {
        eat_byte(iterator);
        table_array = true;
        add_token(iterator, TOKEN_DOUBLE_LBRACKET);
    }
    else
    {
        add_token(iterator, TOKEN_LBRACKET);
        eat_whitespace(iterator);
    }

    lex_key(iterator);

    assert(!match_whitespace(iterator));
    advance(iterator);
    eat_byte(iterator, ']');
    if (table_array)
    {
        eat_byte(iterator, ']');
        add_token(iterator, TOKEN_DOUBLE_RBRACKET);
    }
    else
    {
        add_token(iterator, TOKEN_RBRACKET);
    }
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
        lex_keyval(iterator, LEX_EOL);
    }

    eat_whitespace(iterator);
    eat_comment(iterator);
}


} // namespace


bool
lex_toml(const string &toml, vector<Token> &tokens, vector<Error> &errors)
{
    TomlIterator iterator = { toml, toml.length(), 0, 1, 1, 0, 1, 1, tokens, errors };

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
