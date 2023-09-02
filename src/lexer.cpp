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
constexpr byte BYTE_EOF = INVALID_BYTE;


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
    LEX_HEADER      = 1 << 8,
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
    u64 length;

    u64 start_position;
    u32 start_line;
    u32 start_column;

    u64 current_position;
    u32 current_line;
    u32 current_column;

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


string
get_lexeme(const TomlIterator &iterator)
{
    string result = iterator.toml.substr(
        iterator.start_position,
        iterator.current_position - iterator.start_position);
    return result;
}


void
add_error(TomlIterator &iterator, string message, u64 line, u64 column)
{
    Error error = { line, column, move(message) };
    iterator.errors.push_back(move(error));
    advance(iterator);
}

void
add_error(TomlIterator &iterator, string message)
{
    add_error(iterator, move(message), iterator.start_line, iterator.start_column);
}


void
add_token(TomlIterator &iterator, TokenType type, string lexeme = "")
{
    Token token = { type, nullptr, move(lexeme), iterator.start_position, iterator.start_line, iterator.start_column };
    iterator.tokens.push_back(move(token));
    advance(iterator);
}


void
add_value(TomlIterator &iterator, Value *value)
{
    Token token = { TOKEN_VALUE, value, get_lexeme(iterator), iterator.start_position, iterator.start_line, iterator.start_column };
    iterator.tokens.push_back(move(token));
    advance(iterator);
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


bool
is_binary(byte value)
{
    bool result = (value == '0') || (value == '1');
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
match(const TomlIterator &iterator, byte value, u64 ahead = 0)
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
eat_bytes(TomlIterator &iterator, u64 count, byte expected = INVALID_BYTE)
{
    assert(count);
    for (u64 i = 0; i < count; ++i)
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
eat_string(TomlIterator &iterator, const string &expected, u32 context)
{
    bool result = false;

    string eaten;
    bool eating = true;
    while (eating && !match_eol(iterator) && !match_whitespace(iterator))
    {
        byte c = get_byte(iterator);
        switch (c)
        {
            case ',':
            {
                eating = !(context & (LEX_ARRAY | LEX_TABLE));
            } break;

            case ']':
            {
                eating = !(context & LEX_ARRAY);
            } break;

            case '}':
            {
                eating = !(context & LEX_TABLE);
            } break;
        }

        if (eating)
        {
            eaten.push_back(c);
            eat_byte(iterator);
        }
    }

    if (eaten == expected)
    {
        result = true;
    }
    else
    {
        add_error(iterator, "Invalid value: " + get_lexeme(iterator));
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
missing_key(TomlIterator &iterator)
{
    add_error(iterator, "Missing key.");
}


void
unterminated_string(TomlIterator &iterator)
{
    add_error(iterator, "Unterminated string.", iterator.current_line, iterator.current_column);
}


bool
validate_digits(TomlIterator &iterator, const LexDigitResult &result, const string &type, bool no_leading_zero)
{
    bool valid = true;

    if (result.flags & result.INVALID_DIGIT)
    {
        add_error(iterator, "Invalid " + type + " number: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.flags & result.INVALID_UNDERSCORE)
    {
        add_error(iterator, "'_' must separate digits in " + type + " number: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        Error error = { iterator.current_line, iterator.current_column, "Missing " + type + " number." };
        iterator.errors.push_back(move(error));
        valid = false;
    }
    else if (no_leading_zero && (result.digits[0] == '0') && (result.digits.length() > 1))
    {
        Error error = { iterator.start_line, iterator.start_column, "Leading zero is not allowed in " + type + " number." };
        iterator.errors.push_back(move(error));
        valid = false;
    }

    return valid;
}


bool
validate_hour(TomlIterator &iterator, const LexDigitResult &result)
{
    bool valid = true;

    if (result.flags)
    {
        add_error(iterator, "Invalid hour: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing hour");
        valid = false;
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Hour must be two digits");
        valid = false;
    }
    else if ((result.digits[0] > '2') || ((result.digits[0] == '2') && (result.digits[2] > '3')))
    {
        add_error(iterator, "Hour must be between 00 and 23 inclusive");
        valid = false;
    }

    return valid;
}


bool
validate_minute(TomlIterator &iterator, const LexDigitResult &result)
{
    bool valid = true;

    if (result.flags)
    {
        add_error(iterator, "Invalid minute: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing minute");
        valid = false;
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Minute must be two digits");
        valid = false;
    }
    else if (result.digits[0] > '5')
    {
        add_error(iterator, "Minute must be between 00 and 59 inclusive");
        valid = false;
    }

    return valid;
}


bool
validate_second(TomlIterator &iterator, const LexDigitResult &result)
{
    bool valid = true;

    if (result.flags)
    {
        add_error(iterator, "Invalid second: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing second");
        valid = false;
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Second must be two digits");
        valid = false;
    }
    else if (result.digits[0] > '5')
    {
        // TODO support leap seconds?
        add_error(iterator, "Second must be between 00 and 59 inclusive");
        valid = false;
    }

    return valid;
}


bool
validate_year(TomlIterator &iterator, const LexDigitResult &result)
{
    bool valid = true;

    if (result.flags)
    {
        add_error(iterator, "Invalid year: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.digits.length() > 4)
    {
        add_error(iterator, "Year must be four digits");
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing year");
        valid = false;
    }

    return valid;
}


bool
validate_month(TomlIterator &iterator, const LexDigitResult &result)
{
    bool valid = true;

    if (result.flags)
    {
        add_error(iterator, "Invalid month: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Month must be two digits");
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing month");
        valid = false;
    }
    else if (((result.digits[0] == '0') && (result.digits[1] == '0'))
            || ((result.digits[0] == '1') && (result.digits[1] > '2'))
            || (result.digits[0] > '1'))
    {
        add_error(iterator, "Month must be between 01 and 12 inclusive");
        valid = false;
    }

    return valid;
}


bool
validate_day(TomlIterator &iterator, const LexDigitResult &result)
{
    bool valid = true;

    if (result.flags)
    {
        add_error(iterator, "Invalid day: " + get_lexeme(iterator));
        valid = false;
    }
    else if (result.digits.length() > 2)
    {
        add_error(iterator, "Day must be two digits");
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(iterator, "Missing day");
        valid = false;
    }
    else if (((result.digits[0] == '0') && (result.digits[1] == '0'))
            || ((result.digits[0] == '3') && (result.digits[1] > '1'))
            || (result.digits[0] > '3'))
    {
        // TODO Validate day based on month and year while lexing?
        add_error(iterator, "Day must be between 01 and 31 inclusive");
        valid = false;
    }

    return valid;
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
            || ((c == '}') && (context & LEX_TABLE))
            || (c == '#'))
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


bool
lex_exponent(TomlIterator &iterator, u32 context, string &value)
{
    if (match(iterator, '-') || match(iterator, '+'))
    {
        value.push_back(eat_byte(iterator));
    }

    LexDigitResult exponent = lex_digits(iterator, is_decimal, context);
    bool valid = validate_digits(iterator, exponent, "exponential part of decimal", false);
    value += move(exponent.digits);

    return valid;
}


bool
lex_fraction(TomlIterator &iterator, u32 context, string &value)
{
    LexDigitResult fraction = lex_digits(iterator, is_decimal, context | LEX_EXPONENT);
    bool valid = validate_digits(iterator, fraction, "fractional part of decimal", false);
    value += move(fraction.digits);

    if (match(iterator, 'e') || match(iterator, 'E'))
    {
        value.push_back(eat_byte(iterator));
        valid = lex_exponent(iterator, context, value) && valid;
    }

    return valid;
}


bool
lex_time(TomlIterator &iterator, string &value, LexDigitResult &lexed, u32 context)
{
    bool valid = true;

    if (!validate_hour(iterator, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    value += eat_byte(iterator, ':');

    lexed = lex_digits(iterator, is_decimal, context | LEX_TIME | LEX_FRACTION);
    if (!validate_minute(iterator, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    value += eat_byte(iterator, ':');

    lexed = lex_digits(iterator, is_decimal, context | LEX_FRACTION);
    if (!validate_second(iterator, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    if (match(iterator, '.'))
    {
        value += eat_byte(iterator);
        lexed = lex_digits(iterator, is_decimal, context);
        if (lexed.flags)
        {
            add_error(iterator, "Invalid value for fractional seconds: " + get_lexeme(iterator));
            valid = false;
        }
        else if (lexed.digits.length() == 0)
        {
            add_error(iterator, "Missing fractional seconds");
            valid = false;
        }

        value += move(lexed.digits);
    }

    return valid;
}


bool
lex_hour(TomlIterator &iterator, string &value, u32 context)
{
    LexDigitResult lexed = lex_digits(iterator, is_decimal, context | LEX_TIME | LEX_FRACTION);
    bool result = lex_time(iterator, value, lexed, context);
    return result;
}


bool
lex_timezone(TomlIterator &iterator, string &value, u32 context)
{
    bool result = true;

    value += eat_byte(iterator);
    assert(value.back() == '-' || value.back() == '+');

    LexDigitResult lexed = lex_digits(iterator, is_decimal, context | LEX_TIME);
    if (!validate_hour(iterator, lexed))
    {
        result = false;
    }
    value += move(lexed.digits);

    eat_byte(iterator, ':');

    lexed = lex_digits(iterator, is_decimal, context);
    if (!validate_minute(iterator, lexed))
    {
        result = false;
    }
    value += move(lexed.digits);

    return result;
}


void
lex_date(TomlIterator &iterator, string &value, LexDigitResult &lexed, u32 context)
{
    bool valid = true;
    ValueType type = TYPE_LOCAL_DATE;

    if (value.length())
    {
        add_error(iterator, "'" + value + "' sign not allowed for date");
        valid = false;
    }
    if (!validate_year(iterator, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    value += eat_byte(iterator, '-');

    lexed = lex_digits(iterator, is_decimal, context | LEX_DATE | LEX_DATETIME);
    if (!validate_month(iterator, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    value += eat_byte(iterator, '-');

    lexed = lex_digits(iterator, is_decimal, context | LEX_DATETIME);
    if (!validate_day(iterator, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    if (match(iterator, 'T') || match(iterator, 't')
        || (match(iterator, ' ') && !match_eol(iterator, 1) && is_decimal(get_byte(iterator, 1))))
    {
        type = TYPE_LOCAL_DATETIME;
        eat_byte(iterator);
        value += ' ';

        if (!lex_hour(iterator, value, LEX_TIMEZONE))
        {
            valid = false;
        }

        if (match(iterator, 'Z') ||match(iterator, 'z'))
        {
            type = TYPE_OFFSET_DATETIME;
            eat_byte(iterator);
            value += "+0000";
        }
        else if (match(iterator, '+') || match(iterator, '-'))
        {
            type = TYPE_OFFSET_DATETIME;
            if (!lex_timezone(iterator, value, context))
            {
                valid = false;
            }
        }
    }

    if (valid)
    {
        istringstream in{value};
        switch (type)
        {
            case TYPE_LOCAL_DATE:
            {
                LocalDate result;
                in >> date::parse("%Y-%m-%d", result);
                add_value(iterator, new LocalDateValue(move(result)));
            } break;

            case TYPE_LOCAL_DATETIME:
            {
                LocalDateTime result;
                in >> date::parse("%Y-%m-%d %T", result);
                add_value(iterator, new LocalDateTimeValue(move(result)));
            } break;

            case TYPE_OFFSET_DATETIME:
            {
                assert(type == TYPE_OFFSET_DATETIME);
                OffsetDateTime result;
                in >> date::parse("%Y-%m-%d %T%z", result);
                add_value(iterator, new OffsetDateTimeValue(move(result)));
            } break;

            default:
            {
                assert(false);
            } break;
        }
    }
    else
    {
        add_value(iterator, new Value{});
    }
}


void
lex_decimal(TomlIterator &iterator, u32 context, string &value)
{
    LexDigitResult result = lex_digits(iterator, is_decimal, context | LEX_FRACTION | LEX_EXPONENT | LEX_DATE | LEX_TIME);

    if (match(iterator, '.'))
    {
        bool valid = validate_digits(iterator, result, "whole part of decimal", true);
        value += move(result.digits);
        value.push_back(eat_byte(iterator));
        valid = lex_fraction(iterator, context, value) && valid;
        if (valid)
        {
            add_value(iterator, new FloatValue(string_to_f64(value)));
        }
        else
        {
            add_value(iterator, new Value());
        }
    }
    else if (match(iterator, 'e') || match(iterator, 'E'))
    {
        bool valid = validate_digits(iterator, result, "whole part of decimal", true);
        value += move(result.digits);
        value.push_back(eat_byte(iterator));
        valid = lex_exponent(iterator, context, value) && valid;
        if (valid)
        {
            add_value(iterator, new FloatValue(string_to_f64(value)));
        }
        else
        {
            add_value(iterator, new Value());
        }
    }
    else if (match(iterator, '-'))
    {
        lex_date(iterator, value, result, context);
    }
    else if (match(iterator, ':'))
    {
        bool valid = true;
        if (value.length())
        {
            add_error(iterator, "'" + value + "' sign not allowed for time");
            valid = false;
        }
        if (!lex_time(iterator, value, result, context))
        {
            valid = false;
        }

        if (valid)
        {
            // date::parse apparently doesn't work on bare time values
            int hours = atoi(value.c_str());
            int minutes = atoi(value.c_str() + 3);
            int seconds = atoi(value.c_str() + 6);
            int microseconds = 0;
            if (value.length() > 9) {
                value.resize(15, '0');
                microseconds = atoi(value.c_str() + 9);
            }

            LocalTime lexed{
                chrono::hours{hours}
                + chrono::minutes{minutes}
                + chrono::seconds{seconds} + chrono::microseconds{microseconds}};
            add_value(iterator, new LocalTimeValue(move(lexed)));
        }
        else
        {
            add_value(iterator, new Value());
        }
    }
    else
    {
        if (validate_digits(iterator, result, "decimal", true))
        {
            value += move(result.digits);
            add_value(iterator, new IntegerValue(string_to_s64(value)));
        }
        else
        {
            add_value(iterator, new Value());
        }
    }
}


void
lex_hexadecimal(TomlIterator &iterator, u32 context, const string &value)
{
    bool valid = true;
    if (value.length())
    {
        add_error(iterator, "'" + value + "' sign not allowed for hexadecimal integer");
        valid = false;
    }

    LexDigitResult result = lex_digits(iterator, is_hexadecimal, context);
    if (!validate_digits(iterator, result, "hexadecimal", false))
    {
        valid = false;
    }

    if (valid)
    {
        add_value(iterator, new IntegerValue(string_to_s64(result.digits, nullptr, 16)));
    }
    else
    {
        add_value(iterator, new Value());
    }
}


void
lex_octal(TomlIterator &iterator, u32 context, const string &value)
{
    bool valid = true;

    if (value.length())
    {
        add_error(iterator, "'" + value + "' sign not allowed for octal integer");
        valid = false;
    }

    LexDigitResult result = lex_digits(iterator, is_octal, context);
    if (!validate_digits(iterator, result, "octal", false))
    {
        valid = false;
    }

    if (valid)
    {
        add_value(iterator, new IntegerValue(string_to_s64(result.digits, nullptr, 8)));
    }
    else
    {
        add_value(iterator, new Value());
    }
}


void
lex_binary(TomlIterator &iterator, u32 context, const string &value)
{
    bool valid = true;

    if (value.length())
    {
        add_error(iterator, "'" + value + "' sign not allowed for binary integer");
        valid = false;
    }

    LexDigitResult result = lex_digits(iterator, is_binary, context);
    if (!validate_digits(iterator, result, "binary", false))
    {
        valid = false;
    }

    if (valid)
    {
        add_value(iterator, new IntegerValue(string_to_s64(result.digits, nullptr, 2)));
    }
    else
    {
        add_value(iterator, new Value());
    }
}


void
lex_number(TomlIterator &iterator, u32 context)
{
    string result = get_lexeme(iterator);
    assert((result == "") || (result == "-") || (result == "+"));

    if (match(iterator, '0'))
    {
        if (match(iterator, 'x', 1))
        {
            eat_bytes(iterator, 2);
            lex_hexadecimal(iterator, context, result);
        }
        else if (match(iterator, 'o', 1))
        {
            eat_bytes(iterator, 2);
            lex_octal(iterator, context, result);
        }
        else if (match(iterator, 'b', 1))
        {
            eat_bytes(iterator, 2);
            lex_binary(iterator, context, result);
        }
        else
        {
            lex_decimal(iterator, context, result);
        }
    }
    else
    {
        lex_decimal(iterator, context, result);
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
            add_error(iterator, message.str());
        }
        else if ((nbytes == 3) && (codepoint < 0x800))
        {
            u32 expected_bytes = (codepoint < 0x80) ? 1 : 2;

            ostringstream message;
            message << "Overlong encoding of Unicode value: ";
            format_unicode(message, codepoint);
            message << " is a " << expected_bytes << "-byte value but was encoded in 3 bytes.";
            add_error(iterator, message.str());
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
            add_error(iterator, message.str());
        }
        else if (codepoint > 0x10ffff)
        {
            ostringstream message;
            message << "Invalid Unicode value: ";
            format_unicode(message, codepoint);
            message << " (maximum allowed value is ";
            format_unicode(message, 0x10ffff);
            message << ")";
            add_error(iterator, message.str());
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
            add_error(iterator, message.str());
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
    assert(!end_of_file(iterator));

    byte c = get_byte(iterator);
    switch (c)
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
        if (match_eol(iterator))
        {
            unterminated_string(iterator);
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
                    add_value(iterator, new StringValue(move(value)));
                }
                else
                {
                    string value = lex_string(iterator, c);
                    add_value(iterator, new StringValue(move(value)));
                }
            } break;

            case '-':
            {
                eat_byte(iterator);
                if (match(iterator, 'i'))
                {
                    eat_string(iterator, "inf", context);
                    add_value(iterator, new FloatValue(-INF64));
                }
                else if (match(iterator, 'n'))
                {
                    eat_string(iterator, "nan", context);
                    add_value(iterator, new FloatValue(-NAN64));
                }
                else
                {
                    lex_number(iterator, context);
                }
            } break;

            case '+':
            {
                eat_byte(iterator);
                if (match(iterator, 'i'))
                {
                    eat_string(iterator, "inf", context);
                    add_value(iterator, new FloatValue(+INF64));
                }
                else if (match(iterator, 'n'))
                {
                    eat_string(iterator, "nan", context);
                    add_value(iterator, new FloatValue(+NAN64));
                }
                else
                {
                    lex_number(iterator, context);
                }
            } break;

            case 'i':
            {
                eat_string(iterator, "inf", context);
                add_value(iterator, new FloatValue(INF64));
            } break;

            case 'n':
            {
                eat_string(iterator, "nan", context);
                add_value(iterator, new FloatValue(NAN64));
            } break;


            case 't':
            {
                eat_string(iterator, "true", context);
                add_value(iterator, new BooleanValue(true));
            } break;

            case 'f':
            {
                eat_string(iterator, "false", context);
                add_value(iterator, new BooleanValue(false));
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
lex_unquoted_key(TomlIterator &iterator, u32 context)
{
    assert((context & (LEX_TABLE | LEX_HEADER)) == context);

    string value;
    bool valid = true;
    bool lexing = true;
    while (lexing)
    {
        byte b = get_byte(iterator);
        if (is_key_char(b))
        {
            value += eat_byte(iterator);
        }
        else
        {
            switch (b)
            {
                case BYTE_EOF:
                case ' ':
                case '\t':
                case '\n':
                case '.':
                {
                    lexing = false;
                } break;

                case '\r':
                {
                    lexing = !match(iterator, '\n', 1);
                } break;

                case '=':
                {
                    // headers don't have key-value pairs
                    lexing = context & LEX_HEADER;
                } break;

                case '#':
                {
                    // headers and inline tables cannot have comments
                    lexing = context & (LEX_TABLE | LEX_HEADER);
                } break;

                case ',':
                {
                    lexing = !(context & LEX_TABLE);
                } break;

                case ']':
                {
                    lexing = !(context & LEX_HEADER);
                } break;

                case '}':
                {
                    lexing = !(context & LEX_TABLE);
                } break;
            }

            if (lexing)
            {
                value += eat_byte(iterator);
                valid = false;
            }
        }
    }

    if (!valid)
    {
        add_error(iterator, "Invalid key: " + value);
    }
    else if (value.length() == 0)
    {
        missing_key(iterator);
    }

    add_token(iterator, TOKEN_KEY, move(value));
}


void
lex_simple_key(TomlIterator &iterator, u32 context)
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
        lex_unquoted_key(iterator, context);
    }
}


void
lex_key(TomlIterator &iterator, u32 context)
{
    bool lexing = true;
    while (lexing)
    {
        advance(iterator);
        lex_simple_key(iterator, context);

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
    lex_key(iterator, context);

    eat_whitespace(iterator);

    if (match(iterator, '='))
    {
        eat_byte(iterator);
    eat_whitespace(iterator);
    }
    else
    {
        add_error(iterator, "Missing '=' between key and value.");
    }

    lex_value(iterator, context);
}


void
lex_table(TomlIterator &iterator)
{
    advance(iterator);

    eat_byte(iterator, '[');
    bool table_array = match(iterator, '[');

    if (table_array)
    {
        eat_byte(iterator);
        add_token(iterator, TOKEN_DOUBLE_LBRACKET);
    }
    else
    {
        add_token(iterator, TOKEN_LBRACKET);
        eat_whitespace(iterator);
    }

    lex_key(iterator, LEX_HEADER);

    assert(!match_whitespace(iterator));
    advance(iterator);

    if (match(iterator, ']'))
    {
        eat_byte(iterator);
        if (!table_array)
        {
            add_token(iterator, TOKEN_RBRACKET);
        }
        else if (match(iterator, ']'))
        {
            eat_byte(iterator);
            add_token(iterator, TOKEN_DOUBLE_RBRACKET);
        }
        else
        {
            add_error(iterator, "Missing closing ']' for array of tables.");
        }
    }
    else if (table_array)
    {
        add_error(iterator, "Missing closing ']]' for array of tables.");
    }
    else
    {
        add_error(iterator, "Missing closing ']' for table.");
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
            resynchronize(iterator, "Expected the end of the line but got: ");
        }
    }

    advance(iterator);
    add_token(iterator, TOKEN_EOF);

    return errors.size() == 0;
}


} // namespace toml
