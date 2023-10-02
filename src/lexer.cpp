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


enum ContainerState
{
    CONTAINER_START,
    CONTAINER_VALUE,
    CONTAINER_SEPARATOR,
};


struct LexedDigits
{
    enum Status
    {
        NONE,

        FRACTION,
        EXPONENT,
        DATE,
        TIME,
        TIMEZONE,
        ZULU,
        DONE,

        INVALID,
    };

    string digits;
    SourceLocation start;
    Status result;
};


//
// Predeclarations
//

void
advance(Lexer &lexer, u64 count);

bool
lex_string_char(Lexer &lexer, string &result);

Token
lex_value(Lexer &lexer, u32 context);


//
// Implementation
//


#define string_to_f64 std::stod


s64
string_to_s64(const std::string &value, int base = 10)
{
    s64 result = stoll(value, nullptr, base);
    return result;
}


void
byte_to_hex(byte value, string &out)
{
    out += "0x";
    out.push_back(HEX2CHAR[(value >> 4) & 0xf]);
    out.push_back(HEX2CHAR[value & 0xf]);
}


string
get_lexeme(const Lexer &lexer)
{
    string result = get_lexeme(lexer, lexer.start.index);
    return result;
}


void
add_error(Lexer &lexer, string message, const SourceLocation &location)
{
    Error error = { location, move(message) };

    auto insert_at = lexer.errors.end();
    for ( ; insert_at != lexer.errors.begin(); --insert_at)
    {
        auto check = insert_at - 1;
        if (check->location.index <= error.location.index)
        {
            break;
        }
    }

    lexer.errors.insert(insert_at, move(error));
}

void
add_error(Lexer &lexer, string message)
{
    add_error(lexer, move(message), lexer.start);
}


Token
make_token(Lexer &lexer, TokenType type, u64 length)
{
    advance(lexer, length);
    Token result = { type, Value(), get_lexeme(lexer), lexer.start };

    return result;
}


Token
make_token(Lexer &lexer, TokenType type, string lexeme = "")
{
    Token result = { type, Value(), move(lexeme), lexer.start };
    return result;
}


Token
make_value(Lexer &lexer, Value value)
{
    Token result = { TOKEN_VALUE, std::move(value), get_lexeme(lexer), lexer.start };
    return result;
}


bool
end_of_file(const Lexer &lexer, u64 ahead = 0)
{
    assert(lexer.current.index <= lexer.length);
    bool result = lexer.length - lexer.current.index <= ahead;
    return result;
}


byte
peek(const Lexer &lexer, u64 ahead = 0)
{
    byte result = INVALID_BYTE;
    if (!end_of_file(lexer, ahead))
    {
        result = lexer.toml[lexer.current.index + ahead];
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
match(const Lexer &lexer, byte value, u64 ahead = 0)
{
    bool result = peek(lexer, ahead) == value;
    return result;
}


bool
match_eol(const Lexer &lexer, u64 ahead = 0)
{
    bool result = false;

    if (end_of_file(lexer, ahead) || match(lexer, '\n', ahead))
    {
        result = true;
    }
    else if (match(lexer, '\r', ahead) && match(lexer, '\n', ahead + 1))
    {
        result = true;
    }

    return result;
}


bool
match_whitespace(Lexer &lexer, u64 ahead = 0)
{
    bool result = match(lexer, ' ', ahead) || match(lexer, '\t', ahead);
    return result;
}


void
advance(Lexer &lexer)
{
    assert(!end_of_file(lexer));

    if (lexer.toml[lexer.current.index] == '\n')
    {
        ++lexer.current.line;
        lexer.current.column = 1;
    }
    else
    {
        ++lexer.current.column;
    }

    ++lexer.current.index;
}


void
advance(Lexer &lexer, u64 count)
{
    for (u64 i = 0; i < count; ++i)
    {
        advance(lexer);
    }
}


byte
eat(Lexer &lexer)
{
    assert(!end_of_file(lexer));
    advance(lexer);

    byte result = lexer.toml[lexer.current.index - 1];
    return result;
}


void
eat_comment(Lexer &lexer)
{
    if (match(lexer, '#'))
    {
        advance(lexer);
        string eaten;
        while (!match_eol(lexer))
        {
            lex_string_char(lexer, eaten);
        };
    }
}


Token
make_comment(Lexer &lexer)
{
    eat_comment(lexer);
    Token result = make_token(lexer, TOKEN_COMMENT, get_lexeme(lexer));
    return result;
}


bool
eat_newline(Lexer &lexer)
{
    bool result = false;

    if (match(lexer, '\n'))
    {
        advance(lexer);
        result = true;
    }
    else if (match(lexer, '\r') && match(lexer, '\n', 1))
    {
        advance(lexer, 2);
        result = true;
    }

    return result;
}


bool
eat_whitespace(Lexer &lexer)
{
    bool result = false;

    if (match_whitespace(lexer))
    {
        result = true;
        do
        {
            advance(lexer);
        } while (match_whitespace(lexer));
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


string
format_unicode(u32 codepoint)
{
    ostringstream o;
    o << "U+" << setw(4) << setfill('0') << uppercase << hex << codepoint;

    string result = o.str();
    return result;
}


void
invalid_escape(Lexer &lexer, const SourceLocation &location)
{
    string message = "Invalid escape sequence.";
    Error error = { location, move(message) };
    lexer.errors.push_back(move(error));
}


void
invalid_unicode(Lexer &lexer, const SourceLocation &location)
{
    string message = "Unicode escape sequence specified an invalid or non-allowed codepoint.";
    Error error = { location, move(message) };
    lexer.errors.push_back(move(error));
}


void
invalid_unicode_escape(Lexer &lexer, u32 lexed, u32 count, const SourceLocation &location)
{
    string message = "Invalid or incomplete Unicode escape sequence: expected " + to_string(count) + " hexadecimal characters but parsed " + to_string(lexed) + ".";
    Error error = { location, move(message) };
    lexer.errors.push_back(move(error));
}


void
missing_key(Lexer &lexer)
{
    add_error(lexer, "Missing key.");
}


void
unterminated_string(Lexer &lexer)
{
    add_error(lexer, "Unterminated string.", lexer.current);
}


bool
validate_hour(Lexer &lexer, const LexedDigits &result)
{
    bool valid = true;

    if (result.result == LexedDigits::INVALID)
    {
        add_error(lexer, "Invalid hour: " + get_lexeme(lexer), result.start);
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(lexer, "Missing hour", result.start);
        valid = false;
    }
    else if (result.digits.length() != 2)
    {
        add_error(lexer, "Hour must be two digits.", result.start);
        valid = false;
    }
    else if ((result.digits[0] > '2') || ((result.digits[0] == '2') && (result.digits[1] > '3')))
    {
        add_error(lexer, "Hour must be between 00 and 23 inclusive but value was: " + result.digits, result.start);
        valid = false;
    }

    return valid;
}


bool
validate_minute(Lexer &lexer, const LexedDigits &result)
{
    bool valid = true;

    if (result.result == LexedDigits::INVALID)
    {
        add_error(lexer, "Invalid minute: " + get_lexeme(lexer), result.start);
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(lexer, "Missing minute.", result.start);
        valid = false;
    }
    else if (result.digits.length() != 2)
    {
        add_error(lexer, "Minute must be two digits.", result.start);
        valid = false;
    }
    else if (result.digits[0] > '5')
    {
        add_error(lexer, "Minute must be between 00 and 59 inclusive but value was: " + result.digits, result.start);
        valid = false;
    }

    return valid;
}


bool
validate_second(Lexer &lexer, const LexedDigits &result)
{
    bool valid = true;

    if (result.result == LexedDigits::INVALID)
    {
        add_error(lexer, "Invalid second: " + get_lexeme(lexer), result.start);
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(lexer, "Missing second.", result.start);
        valid = false;
    }
    else if (result.digits.length() != 2)
    {
        add_error(lexer, "Second must be two digits.", result.start);
        valid = false;
    }
    else if (result.digits[0] > '5')
    {
        // TODO support leap seconds?
        add_error(lexer, "Second must be between 00 and 59 inclusive but value was: " + result.digits, result.start);
        valid = false;
    }

    return valid;
}


bool
validate_year(Lexer &lexer, const LexedDigits &result)
{
    bool valid = true;

    if (result.result == LexedDigits::INVALID)
    {
        add_error(lexer, "Invalid year: " + get_lexeme(lexer), result.start);
        valid = false;
    }
    else if (result.digits.length() != 4)
    {
        add_error(lexer, "Year must be four digits", result.start);
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(lexer, "Missing year", result.start);
        valid = false;
    }

    return valid;
}


bool
validate_month(Lexer &lexer, const LexedDigits &result)
{
    bool valid = true;

    if (result.result == LexedDigits::INVALID)
    {
        add_error(lexer, "Invalid month: " + get_lexeme(lexer), result.start);
        valid = false;
    }
    else if (result.digits.length() != 2)
    {
        add_error(lexer, "Month must be two digits.", result.start);
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(lexer, "Missing month", result.start);
        valid = false;
    }
    else if (((result.digits[0] == '0') && (result.digits[1] == '0'))
            || ((result.digits[0] == '1') && (result.digits[1] > '2'))
            || (result.digits[0] > '1'))
    {
        add_error(lexer, "Month must be between 01 and 12 inclusive but value was: " + result.digits, result.start);
        valid = false;
    }

    return valid;
}


bool
validate_day(Lexer &lexer, const LexedDigits &result)
{
    bool valid = true;

    if (result.result == LexedDigits::INVALID)
    {
        add_error(lexer, "Invalid day: " + result.digits, result.start);
        valid = false;
    }
    else if (result.digits.length() != 2)
    {
        add_error(lexer, "Day must be two digits.", result.start);
        valid = false;
    }
    else if (result.digits.length() == 0)
    {
        add_error(lexer, "Missing day", result.start);
        valid = false;
    }
    else if (((result.digits[0] == '0') && (result.digits[1] == '0'))
            || ((result.digits[0] == '3') && (result.digits[1] > '1'))
            || (result.digits[0] > '3'))
    {
        // TODO Validate day based on month and year while lexing?
        add_error(lexer, "Day must be between 01 and 31 inclusive but value was: " + result.digits, result.start);
        valid = false;
    }

    return valid;
}


Token
lex_literal(Lexer &lexer, const string &expected, Value value, u32 context)
{
    string eaten;
    bool eating = true;
    while (eating && !match_eol(lexer) && !match_whitespace(lexer))
    {
        byte c = peek(lexer);
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
            advance(lexer);
        }
    }

    if (eaten != expected)
    {
        add_error(lexer, "Invalid value: " + get_lexeme(lexer));
    }

    Token result = make_value(lexer, move(value));
    return result;
}


LexedDigits
lex_digits(Lexer &lexer, IsDigit is_digit, u32 context)
{
    LexedDigits lexed;
    lexed.start = lexer.current;

    bool valid = true;
    bool has_underscore = false;
    bool underscore_ok = false;
    bool lexing = true;
    while (lexing)
    {
        if (match_eol(lexer) || match_whitespace(lexer))
        {
            lexing = false;
            lexed.result = LexedDigits::DONE;
        }
        else
        {
            byte b = peek(lexer);
            if (is_digit(b))
            {
                advance(lexer);
                lexed.digits.push_back(b);
                underscore_ok = true;
            }
            else
            {
                switch (b)
                {
                    case '_':
                    {
                        advance(lexer);
                        has_underscore = true;
                        if (underscore_ok)
                        {
                            underscore_ok = false;
                        }
                        else
                        {
                            valid = false;
                        }
                        continue;
                    } break;

                    case '#':
                    {
                        lexing = false;
                        lexed.result = LexedDigits::DONE;
                    } break;

                    case '.':
                    {
                        if (context & (LEX_NUMBER | LEX_TIME))
                        {
                            lexing = false;
                            lexed.result = LexedDigits::FRACTION;
                        }
                    } break;

                    case 'e':
                    case 'E':
                    {
                        if (context & LEX_NUMBER)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::EXPONENT;
                        }
                    } break;

                    case '-':
                    {
                        if (context & LEX_DATE)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::DATE;
                        }
                        else if (context & LEX_TIME)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::TIMEZONE;
                        }
                    } break;

                    case ':':
                    {
                        if (context & LEX_TIME)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::TIME;
                        }
                    } break;

                    case 'T':
                    case 't':
                    {
                        if (context & LEX_DATE)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::TIME;
                        }
                    } break;

                    case '+':
                    {
                        if (context & LEX_TIME)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::TIMEZONE;
                        }
                    } break;

                    case 'Z':
                    case 'z':
                    {
                        if (context & LEX_TIME)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::ZULU;
                        }
                    } break;

                    case ',':
                    {
                        if (context & (LEX_ARRAY | LEX_TABLE))
                        {
                            lexing = false;
                            lexed.result = LexedDigits::DONE;
                        }
                    } break;

                    case ']':
                    {
                        if (context & LEX_ARRAY)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::DONE;
                        }
                    } break;

                    case '}':
                    {
                        if (context & LEX_TABLE)
                        {
                            lexing = false;
                            lexed.result = LexedDigits::DONE;
                        }
                    } break;
                }

                if (lexing)
                {
                    valid = false;
                    lexed.digits.push_back(b);
                    advance(lexer);
                }
            }
        }
    }

    if (lexed.digits.length() == 0)
    {
        lexed.result = LexedDigits::NONE;
    }
    else if (!valid
            // check for trailing underscore
            || (has_underscore && !underscore_ok))
    {
        lexed.result = LexedDigits::INVALID;
    }
    else if (has_underscore)
    {
        switch (lexed.result)
        {
            case LexedDigits::DATE:
            case LexedDigits::TIME:
            case LexedDigits::TIMEZONE:
            case LexedDigits::ZULU:
            {
                lexed.result = LexedDigits::INVALID;
            } break;

            default:
            {
                if (!(context & LEX_NUMBER))
                {
                    lexed.result = LexedDigits::INVALID;
                }
            } break;
        }
    }

    return lexed;
}


bool
lex_time(Lexer &lexer, string &value, LexedDigits &lexed, u32 context)
{
    bool valid = true;

    if (!validate_hour(lexer, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    if (match(lexer, ':'))
    {
        value.push_back(eat(lexer));

        lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
        if (!validate_minute(lexer, lexed))
        {
            valid = false;
        }
        value += move(lexed.digits);

        if (match(lexer, ':'))
        {
            value.push_back(eat(lexer));

            lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
            if (!validate_second(lexer, lexed))
            {
                valid = false;
            }
            value += move(lexed.digits);

            if (match(lexer, '.'))
            {
                value.push_back(eat(lexer));
                lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
                if (lexed.result == LexedDigits::INVALID)
                {
                    add_error(lexer, "Invalid value for fractional seconds: " + get_lexeme(lexer), lexed.start);
                    valid = false;
                }
                else if (lexed.digits.length() == 0)
                {
                    add_error(lexer, "Missing fractional seconds", lexed.start);
                    valid = false;
                }

                value += move(lexed.digits);
            }
        }
        else
        {
            valid = false;
            add_error(lexer, "Invalid or missing seconds for time.", lexer.current);
            resynchronize(lexer, context);
        }
    }
    else
    {
        valid = false;
        add_error(lexer, "Invalid or missing minutes for time.", lexer.current);
        resynchronize(lexer, context);
    }

    return valid;
}


bool
lex_hour(Lexer &lexer, string &value, u32 context)
{
    bool result;
    LexedDigits lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
    if (lexed.digits.length())
    {
        result = lex_time(lexer, value, lexed, context);
    }
    else
    {
        result = false;
        add_error(lexer, "Missing time.", lexed.start);
    }
    return result;
}


bool
lex_timezone(Lexer &lexer, string &value, u32 context)
{
    bool result = true;

    value.push_back(eat(lexer));
    assert(value.back() == '-' || value.back() == '+');

    LexedDigits lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
    if (!validate_hour(lexer, lexed))
    {
        result = false;
    }
    value += move(lexed.digits);

    assert(match(lexer, ':'));
    advance(lexer);

    lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
    if (!validate_minute(lexer, lexed))
    {
        result = false;
    }
    value += move(lexed.digits);

    return result;
}


Token
lex_date(Lexer &lexer, string &value, LexedDigits &lexed, u32 context)
{
    bool valid = true;
    Value::Type type = Value::Type::LOCAL_DATE;

    if (value.length())
    {
        add_error(lexer, "'" + value + "' sign not allowed for date");
        valid = false;
    }
    if (!validate_year(lexer, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    assert(match(lexer, '-'));
    value.push_back(eat(lexer));

    lexed = lex_digits(lexer, is_decimal, context | LEX_DATE);
    if (!validate_month(lexer, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    assert(match(lexer, '-'));
    value.push_back(eat(lexer));

    lexed = lex_digits(lexer, is_decimal, context | LEX_DATE);
    if (!validate_day(lexer, lexed))
    {
        valid = false;
    }
    value += move(lexed.digits);

    if (match(lexer, 'T') || match(lexer, 't')
        || (match(lexer, ' ') && !match_eol(lexer, 1) && is_decimal(peek(lexer, 1))))
    {
        type = Value::Type::LOCAL_DATETIME;
        advance(lexer);
        value += ' ';

        if (!lex_hour(lexer, value, LEX_TIMEZONE))
        {
            valid = false;
        }

        if (match(lexer, 'Z') ||match(lexer, 'z'))
        {
            type = Value::Type::OFFSET_DATETIME;
            advance(lexer);
            value += "+0000";
        }
        else if (match(lexer, '+') || match(lexer, '-'))
        {
            type = Value::Type::OFFSET_DATETIME;
            if (!lex_timezone(lexer, value, context))
            {
                valid = false;
            }
        }
    }

    Token token;
    if (valid)
    {
        istringstream in{value};
        switch (type)
        {
            case Value::Type::LOCAL_DATE:
            {
                LocalDate result;
                in >> date::parse("%Y-%m-%d", result);
                token = make_value(lexer, Value(result));
            } break;

            case Value::Type::LOCAL_DATETIME:
            {
                LocalDateTime result;
                in >> date::parse("%Y-%m-%d %T", result);
                token = make_value(lexer, Value(result));
            } break;

            case Value::Type::OFFSET_DATETIME:
            {
                assert(type == Value::Type::OFFSET_DATETIME);
                OffsetDateTime result;
                in >> date::parse("%Y-%m-%d %T%z", result);
                token = make_value(lexer, Value(result));
            } break;

            default:
            {
                assert(false);
            } break;
        }
    }
    else
    {
        token = make_value(lexer, Value());
    }

    return token;
}


Token
lex_exponent(Lexer &lexer, string &value, LexedDigits &lexed, u32 context)
{
    assert(match(lexer, 'e') || match(lexer, 'E'));
    value.push_back(eat(lexer));

    if (match(lexer, '-') || match(lexer, '+'))
    {
        value.push_back(eat(lexer));
    }

    lexed = lex_digits(lexer, is_decimal, context | LEX_NUMBER);

    Token result;
    if (lexed.result == LexedDigits::DONE)
    {
        value += move(lexed.digits);
        result = make_value(lexer, Value(string_to_f64(value)));
    }
    else
    {
        resynchronize(lexer, context);
        add_error(lexer, "Invalid value: " + get_lexeme(lexer));
        result = make_value(lexer, Value());
    }

    return result;
}


Token
lex_fraction(Lexer &lexer, string &value, LexedDigits &lexed, u32 context)
{
    assert(match(lexer, '.'));
    value.push_back(eat(lexer));

    lexed = lex_digits(lexer, is_decimal, context | LEX_NUMBER);

    Token result;
    switch (lexed.result)
    {
        case LexedDigits::DONE:
        {
            value += move(lexed.digits);
            result = make_value(lexer, Value(string_to_f64(value)));
        } break;

        case LexedDigits::EXPONENT:
        {
            value += move(lexed.digits);
            result = lex_exponent(lexer, value, lexed, context);
        } break;

        default:
        {
            resynchronize(lexer, context);
            add_error(lexer, "Invalid value: " + get_lexeme(lexer));
            result = make_value(lexer, Value());
        } break;
    }

    return result;
}


Token
lex_number(Lexer &lexer, string &value, LexedDigits &lexed, u32 context)
{
    LexedDigits integer = lexed;

    Token result;
    switch (lexed.result)
    {
        case LexedDigits::DONE:
        {
            value += move(lexed.digits);
            result = make_value(lexer, Value(string_to_s64(value)));
        } break;

        case LexedDigits::FRACTION:
        {
            value += move(lexed.digits);
            result = lex_fraction(lexer, value, lexed, context);
        } break;

        case LexedDigits::EXPONENT:
        {
            value += move(lexed.digits);
            result = lex_exponent(lexer, value, lexed, context);
        } break;

        default:
        {
            resynchronize(lexer, context);
            add_error(lexer, "Invalid value: " + get_lexeme(lexer));
            result = make_value(lexer, Value());
        } break;
    }

    if ((result.value.type() != Value::Type::INVALID)
        && (integer.digits[0] == '0') && (integer.digits.length() > 1))
    {
        add_error(lexer, "Number may not have leading zeros.", integer.start);
    }

    return result;
}


Token
lex_decimal(Lexer &lexer, u32 context, string &value)
{
    Token result;
    LexedDigits lexed = lex_digits(lexer, is_decimal, context | LEX_NUMBER | LEX_DATE | LEX_TIME);

    switch (lexed.result)
    {
        case LexedDigits::DONE:
        case LexedDigits::FRACTION:
        case LexedDigits::EXPONENT:
        {
            result = lex_number(lexer, value, lexed, context);
        } break;

        case LexedDigits::DATE:
        {
            result = lex_date(lexer, value, lexed, context);
        } break;

        case LexedDigits::TIME:
        {
            if (lex_time(lexer, value, lexed, context))
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

                LocalTime localtime{
                    chrono::hours{hours}
                    + chrono::minutes{minutes}
                    + chrono::seconds{seconds} + chrono::microseconds{microseconds}};
                result = make_value(lexer, Value(localtime));
            }
            else
            {
                result = make_value(lexer, Value());
            }
        } break;

        default:
        {
            resynchronize(lexer, context);
            add_error(lexer, "Invalid value: " + get_lexeme(lexer));
            result = make_value(lexer, Value());
        } break;
    }

    return result;
}


Token
lex_hexadecimal(Lexer &lexer, const string &value, u32 context)
{
    advance(lexer, 2);
    LexedDigits lexed = lex_digits(lexer, is_hexadecimal, context | LEX_NUMBER);

    Token result;
    if (lexed.result == LexedDigits::DONE)
    {
        if (value.length())
        {
            add_error(lexer, "Hexadecimal number may not have a leading sign.");
        }
        result = make_value(lexer, Value(string_to_s64(lexed.digits, 16)));
    }
    else
    {
        resynchronize(lexer, context);
        add_error(lexer, "Invalid hexadecimal number: " + get_lexeme(lexer));
        result = make_value(lexer, Value());
    }

    return result;
}


Token
lex_octal(Lexer &lexer, const string &value, u32 context)
{
    advance(lexer, 2);
    LexedDigits lexed = lex_digits(lexer, is_octal, context | LEX_NUMBER);

    Token result;
    if (lexed.result == LexedDigits::DONE)
    {
        if (value.length())
        {
            add_error(lexer, "Octal number may not have a leading sign.");
        }
        result = make_value(lexer, Value(string_to_s64(lexed.digits, 8)));
    }
    else
    {
        resynchronize(lexer, context);
        add_error(lexer, "Invalid octal number: " + get_lexeme(lexer));
        result = make_value(lexer, Value());
    }

    return result;
}


Token
lex_binary(Lexer &lexer, const string &value, u32 context)
{
    advance(lexer, 2);
    LexedDigits lexed = lex_digits(lexer, is_binary, context | LEX_NUMBER);

    Token result;
    if (lexed.result == LexedDigits::DONE)
    {
        if (value.length())
        {
            add_error(lexer, "Binary number may not have a leading sign.");
        }
        result = make_value(lexer, Value(string_to_s64(lexed.digits, 2)));
    }
    else
    {
        resynchronize(lexer, context);
        add_error(lexer, "Invalid binary number: " + get_lexeme(lexer));
        result = make_value(lexer, Value());
    }

    return result;
}


Token
lex_numeric(Lexer &lexer, u32 context)
{
    Token result;

    string value = get_lexeme(lexer);
    assert((value == "") || (value == "-") || (value == "+"));

    if (match(lexer, '0'))
    {
        if (match(lexer, 'x', 1))
        {
            result = lex_hexadecimal(lexer, value, context);
        }
        else if (match(lexer, 'o', 1))
        {
            result = lex_octal(lexer, value, context);
        }
        else if (match(lexer, 'b', 1))
        {
            result = lex_binary(lexer, value, context);
        }
        else
        {
            result = lex_decimal(lexer, context, value);
        }
    }
    else if (match(lexer, 'i'))
    {
        double inf = INF64;
        if (value == "-")
        {
            inf *= -1;
        }
        result = lex_literal(lexer, "inf", Value(inf), context);
    }
    else if (match(lexer, 'n'))
    {
        double nan = NAN64;
        if (value == "-")
        {
            nan *= -1;
        }
        result = lex_literal(lexer, "nan", Value(nan), context);
    }
    else
    {
        result = lex_decimal(lexer, context, value);
    }

    return result;
}


bool
lex_string_char(Lexer &lexer, string &result)
{
    assert(!end_of_file(lexer));

    SourceLocation location = lexer.current;
    u32 codepoint = 0;
    u32 nbytes = 0;
    bool valid = true;

    byte c = eat(lexer);
    result.push_back(c);

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
        add_error(lexer, move(message), location);
        nbytes = 1; // Let's just eat the byte and (try to) keep going
        valid = false;
    }

    u32 eaten = 1;
    for ( ; (eaten < nbytes) && valid && !end_of_file(lexer); ++eaten)
    {
        c = peek(lexer);
        if ((c & B11000000) == B10000000)
        {
            byte masked = c & B00111111;
            u32 shift = 6 * (nbytes - 1 - eaten);
            codepoint |= (masked << shift);
            result.push_back(eat(lexer));
        }
        else
        {
            string message = "Invalid UTF-8 continuation byte: ";
            byte_to_hex(c, message);
            message += ". Expected a byte in the range of ";
            byte_to_hex(0x80, message);
            message += '-';
            byte_to_hex(0xbf, message);
            message += '.';
            add_error(lexer, move(message), location);
            valid = false;
        }
    }

    // if eaten < nbytes, then we'll err on an unterminated string
    if (valid)
    {
        if (eaten == nbytes)
        {
            if ((nbytes == 2) && (codepoint < 0x80))
            {
                string message = "Overlong encoding of Unicode codepoint: " + format_unicode(codepoint)
                               + " should be encoded using 1 byte but was encoded using 2.";
                add_error(lexer, message, location);
                valid = false;
            }
            else if ((nbytes == 3) && (codepoint < 0x800))
            {
                string expected_bytes = (codepoint < 0x80) ? "1 byte" : "2 bytes";

                string message = "Overlong encoding of Unicode codepoint: " + format_unicode(codepoint)
                               + " should be encoded using " + expected_bytes + " but was encoded using 3.";
                add_error(lexer, message, location);
                valid = false;
            }
            else if ((nbytes == 4) && (codepoint < 0x10000))
            {
                string expected_bytes = "1 byte";
                if (codepoint < 0x800)
                {
                    expected_bytes = "3 bytes";
                }
                else if (codepoint < 0x80)
                {
                    expected_bytes = "2 bytes";
                }

                string message = "Overlong encoding of Unicode codepoint: " + format_unicode(codepoint)
                               + " should be encoded using " + expected_bytes + " but was encoded using 4.";
                add_error(lexer, message, location);
                valid = false;
            }
            else if (codepoint > 0x10ffff)
            {
                string message = "Invalid Unicode codepoint: " + format_unicode(codepoint)
                               + " (maximum codepoint is " + format_unicode(0x10ffff) + ')';
                add_error(lexer, message, location);
                valid = false;
            }
            else if (!(
                (codepoint == 0x09)
                || ((codepoint >= 0x20) && (codepoint <= 0x7e))
                || ((codepoint >= 0x80) && (codepoint <= 0xd7ff))
                || (codepoint >= 0xe000)))
            {
                string message = "Unicode codepoint " + format_unicode(codepoint) + " is not allowed.";
                add_error(lexer, message, location);
                valid = false;
            }

            // Adjust the current column to reflect only 1 "character". Although
            // this probably falls apart with combining characters, etc. Pull
            // requests accepted. :-)
            lexer.current.column -= (nbytes - 1);
        }
        else
        {
            valid = false;
            add_error(lexer, "Unable to decode UTF-8 codepoint: unexpected end of file after decoding " + to_string(eaten) + " of " + to_string(nbytes) + " bytes.");
        }
    }

    return valid;
}


void
lex_unicode(Lexer &lexer, string &result, u32 count)
{
    assert((count == 4) || (count == 8));
    assert(lexer.current.column >= 3);

    SourceLocation location = lexer.current;
    location.index -= 2;
    location.column -= 2;
    u32 codepoint = 0;
    bool valid = true;
    u32 lexed = 0;
    for ( ; (lexed < count) && !end_of_file(lexer); ++lexed)
    {
        byte c = peek(lexer);

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
        advance(lexer);
    }

    if (valid && (lexed == count))
    {
        if ((codepoint <= 0xd7ff) || ((codepoint >= 0xe000) && (codepoint <= 0x10ffff)))
        {
            convert_unicode_to_utf8(codepoint, result);
        }
        else
        {
            invalid_unicode(lexer, location);
        }
    }
    else if (!valid)
    {
        invalid_unicode_escape(lexer, lexed, count, location);
    }
}


void
lex_escape(Lexer &lexer, string &result)
{
    assert(match(lexer, '\\'));

    SourceLocation location = lexer.current;
    advance(lexer);

    // If we've hit the end of the file, then we'll error on an unterminated string
    if (!end_of_file(lexer))
    {
        byte c = eat(lexer);
        switch (c)
        {
            case '"':
            case '\'':
            case '\\':
            {
                result.push_back(c);
            } break;

            case 'b':
            {
                result.push_back('\b');
            } break;

            case 'f':
            {
                result.push_back('\f');
            } break;

            case 'n':
            {
                result.push_back('\n');
            } break;

            case 'r':
            {
                result.push_back('\r');
            } break;

            case 't':
            {
                result.push_back('\t');
            } break;

            case 'u':
            {
                lex_unicode(lexer, result, 4);
            } break;

            case 'U':
            {
                lex_unicode(lexer, result, 8);
            } break;

            default:
            {
                invalid_escape(lexer, location);
                result.push_back(c);
            }
        }
    }
}


string
lex_multiline_string(Lexer &lexer)
{
    byte delimiter = peek(lexer);
    assert((delimiter == '"') || (delimiter == '\''));
    assert(match(lexer, delimiter, 1) && match(lexer, delimiter, 2));

    advance(lexer, 3);
    eat_newline(lexer);

    string result;
    bool lexing = true;
    while (lexing)
    {
        if (end_of_file(lexer))
        {
            unterminated_string(lexer);
            lexing = false;
        }
        else if (eat_newline(lexer))
        {
            result.push_back('\n');
        }
        else
        {
            byte c = peek(lexer);
            if (c == delimiter)
            {
                u32 i = 1;
                for ( ; match(lexer, delimiter, i) && (i < 5); ++i);

                advance(lexer, i);
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

                    default:
                    {
                        assert(false);
                    } break;
                }
            }
            else if ((c == '\\') && (delimiter == '"'))
            {
                u64 ahead = 1;
                while (!end_of_file(lexer, ahead) && match_whitespace(lexer, ahead))
                {
                    ++ahead;
                }
                if (match_eol(lexer, ahead))
                {
                    advance(lexer, ahead);
                    while (eat_newline(lexer))
                    {
                        eat_whitespace(lexer);
                    }
                }
                else
                {
                    lex_escape(lexer, result);
                }
            }
            else
            {
                lex_string_char(lexer, result);
            }
        }
    }

    return result;
}


string
lex_string(Lexer &lexer)
{
    byte delimiter = eat(lexer);
    assert((delimiter == '"') || (delimiter == '\''));

    string result;
    bool lexing = true;
    while (lexing)
    {
        if (match_eol(lexer))
        {
            unterminated_string(lexer);
            lexing = false;
        }
        else
        {
            byte c = peek(lexer);
            if (c == delimiter)
            {
                advance(lexer);
                lexing = false;
            }
            else if ((c == '\\') && (delimiter == '"'))
            {
                lex_escape(lexer, result);
            }
            else
            {
                lex_string_char(lexer, result);
            }
        }
    }

    return result;
}


Token
lex_value(Lexer &lexer, u32 context)
{
    Token result;
    byte c = peek(lexer);
    if (is_decimal(c))
    {
        result = lex_numeric(lexer, context);
    }
    else
    {
        switch (c)
        {
            case '"':
            case '\'':
            {
                if (match(lexer, c, 1) && match(lexer, c, 2))
                {
                    string value = lex_multiline_string(lexer);
                    result = make_value(lexer, Value(move(value)));
                }
                else
                {
                    string value = lex_string(lexer);
                    result = make_value(lexer, Value(move(value)));
                }
            } break;

            case '-':
            case '+':
            {
                advance(lexer);
                result = lex_numeric(lexer, context);
            } break;

            case 'i':
            case 'n':
            {
                result = lex_numeric(lexer, context);
            } break;

            case 't':
            {
                result = lex_literal(lexer, "true", Value(true), context);
            } break;

            case 'f':
            {
                result = lex_literal(lexer, "false", Value(false), context);
            } break;

            default:
            {
                resynchronize(lexer, context);
                add_error(lexer, "Invalid value: " + get_lexeme(lexer));
                result = make_value(lexer, Value());
            } break;
        }
    }

    return result;
}


Token
lex_unquoted_key(Lexer &lexer, u32 context)
{
    string value;
    bool valid = true;
    bool lexing = true;
    while (lexing && !end_of_file(lexer))
    {
        byte b = peek(lexer);

        if (is_key_char(b))
        {
            value.push_back(eat(lexer));
        }
        else
        {
            switch (b)
            {
                case ' ':
                case '\t':
                case '\n':
                case '.':
                case '#':
                {
                    lexing = false;
                } break;

                case '\r':
                {
                    lexing = !match(lexer, '\n', 1);
                } break;

                case '=':
                {
                    lexing = !(context & LEX_KEY);
                } break;

                case ']':
                {
                    lexing = !(context & (LEX_ARRAY_HEADER | LEX_TABLE_HEADER));
                } break;

                case ',':
                case '}':
                {
                    lexing = !(context & LEX_TABLE);
                } break;
            }

            if (lexing)
            {
                if (lex_string_char(lexer, value))
                {
                    valid = false;
                }
            }
        }
    }

    if (!valid)
    {
        add_error(lexer, "Invalid key: " + value);
    }
    else if (value.length() == 0)
    {
        missing_key(lexer);
    }

    Token result = make_token(lexer, TOKEN_KEY, move(value));
    return result;
}


Token
lex_key(Lexer &lexer, u32 context)
{
    // We always return a key token, even if we lex an invalid key. This
    // reduces the number of special cases that the parser needs to handle with
    // the trade-off of potentially generating more key redefinition errors if
    // invalid keys happen to collide with each other or other valid keys. It
    // seems like the chances of this happening would be fairly unlikely, and
    // even if it should occur, it's not evident while writing this that such
    // errors would be completely useless since, depending on the nature of the
    // invalid key(s), the collision might still be present even if all keys
    // are valid. Regardless, we're currently opting for a simpler
    // implementation rather than an arguably more "correct" one.
    Token result;

    byte c = peek(lexer);
    switch (c)
    {
        case '"':
        case '\'':
        {
            string key = lex_string(lexer);
            result = make_token(lexer, TOKEN_KEY, move(key));
        } break;

        default:
        {
            result = lex_unquoted_key(lexer, context);
        } break;
    }

    return result;
}


} // namespace


string
get_lexeme(const Lexer &lexer, u64 from)
{
    string result = lexer.toml.substr(from, lexer.current.index - from);
    return result;
}



Token
next_token(Lexer &lexer, u32 context)
{
    Token result;
    bool lexing = true;
    while (lexing)
    {
        lexer.start = lexer.current;
        if (end_of_file(lexer))
        {
            result = make_token(lexer, TOKEN_EOF, 0);
            lexing = false;
        }
        else
        {
            byte b = peek(lexer);
            switch (b)
            {
                case ' ':
                case '\t':
                {
                    eat_whitespace(lexer);
                } break;

                case '#':
                {
                    result = make_comment(lexer);
                    lexing = false;
                } break;

                case '\n':
                {
                    result = make_token(lexer, TOKEN_NEWLINE, 1);
                    lexing = false;
                } break;

                case '\r':
                {
                    if (match(lexer, '\n', 1))
                    {
                        result = make_token(lexer, TOKEN_NEWLINE, 2);
                        lexing = false;
                    }
                    else
                    {
                        string message = "Unicode codepoint " + format_unicode(b) + " is not allowed.";
                        add_error(lexer, message, lexer.current);
                        advance(lexer);
                    }
                } break;

                case '.':
                {
                    result = make_token(lexer, TOKEN_PERIOD, 1);
                    lexing = false;
                } break;

                case '=':
                {
                    result = make_token(lexer, TOKEN_EQUAL, 1);
                    lexing = false;
                } break;

                case '[':
                {
                    if ((context & LEX_ARRAY_HEADER) && match(lexer, '[', 1))
                    {
                        result = make_token(lexer, TOKEN_DOUBLE_LBRACKET, 2);
                    }
                    else
                    {
                        result = make_token(lexer, TOKEN_LBRACKET, 1);
                    }
                    lexing = false;
                } break;

                case ']':
                {
                    if ((context & LEX_ARRAY_HEADER) && match(lexer, ']', 1))
                    {
                        result = make_token(lexer, TOKEN_DOUBLE_RBRACKET, 2);
                    }
                    else
                    {
                        result = make_token(lexer, TOKEN_RBRACKET, 1);
                    }
                    lexing = false;
                } break;

                case '{':
                {
                    result = make_token(lexer, TOKEN_LBRACE, 1);
                    lexing = false;
                } break;

                case '}':
                {
                    result = make_token(lexer, TOKEN_RBRACE, 1);
                    lexing = false;
                } break;

                case ',':
                {
                    result = make_token(lexer, TOKEN_COMMA, 1);
                    lexing = false;
                } break;

                default:
                {
                    if (context & LEX_VALUE)
                    {
                        result = lex_value(lexer, context);
                    }
                    else if (context & (LEX_KEY | LEX_ARRAY_HEADER | LEX_TABLE_HEADER))
                    {
                        result = lex_key(lexer, context);
                    }
                    else
                    {
                        result = make_token(lexer, TOKEN_NONE);
                    }
                    lexing = false;
                } break;
            }
        }
    }

    return result;
}


void
resynchronize(Lexer &lexer, u32 context)
{
    u64 uneaten = 0;
    bool valid = true;
    bool whitespace = false;
    bool eating = true;
    while (eating && !end_of_file(lexer, uneaten))
    {
        byte b = peek(lexer, uneaten);

        switch (b)
        {
            case '\n':
            {
                eating = false;
            } break;

            case '\r':
            {
                eating = !match(lexer, '\n', 1);
            } break;

            case ' ':
            case '\t':
            {
                ++uneaten;
                whitespace = true;
            } // fallthrough
            case '.':
            {
                eating = !(context & (LEX_KEY | LEX_ARRAY_HEADER | LEX_TABLE_HEADER));
            } break;

            case '=':
            {
                eating = (context & LEX_KEY) == 0;
            } break;

            case '#':
            {
                // headers and inline tables cannot have comments
                eating = context & (LEX_TABLE | LEX_ARRAY_HEADER | LEX_TABLE_HEADER);
            } break;

            case ',':
            {
                eating = !(context & (LEX_TABLE | LEX_ARRAY));
            } break;

            case ']':
            {
                eating = !(context & (LEX_ARRAY | LEX_ARRAY_HEADER | LEX_TABLE_HEADER));
            } break;

            case '}':
            {
                eating = (context & LEX_TABLE) == 0;
            } break;
        }

        if (eating)
        {
            if (whitespace)
            {
                whitespace = false;
            }
            else
            {
                advance(lexer, uneaten);
                uneaten = 0;
                string temp;
                valid = lex_string_char(lexer, temp) && valid;
            }
        }
    }
}


} // namespace toml
