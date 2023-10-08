//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include "lexer.hpp"

#include "common.hpp"
#include "error.hpp"

#include <utility> // move



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


struct LexedDateTimePart
{
    u64 length;
    SourceLocation source;
};


struct LexedDateTime
{
    string &value;
    Value::Type type;
    LexedDateTimePart year;
    LexedDateTimePart month;
    LexedDateTimePart day;
    LexedDateTimePart hour;
    LexedDateTimePart minute;
    LexedDateTimePart second;
    LexedDateTimePart microsecond;
    LexedDateTimePart tz_hour;
    LexedDateTimePart tz_minute;

    LexedDateTime(string &value_, Value::Type type_)
        : value(value_)
        , type(type_)
    {
        assert((type == Value::Type::LOCAL_DATE) || (type == Value::Type::LOCAL_TIME));
    }
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
    else if (codepoint < 0x800)
    {
        nbytes = 2;
        byte c = B11000000 | ((codepoint >> 6) & B00011111);
        out.push_back(c);
    }
    else if (codepoint < 0x10000)
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


bool
validate_second(Lexer &lexer, const string &value, u64 index, const LexedDateTimePart &part)
{
    bool result = true;

    if (part.length != 2)
    {
        second_wrong_length(lexer, part.source);
        result = false;
    }
    else if (value[index] > '5')
    {
        second_out_of_range(lexer, part.source);
        result = false;
    }

    return result;
}


bool
validate_minute(Lexer &lexer, const string &value, u64 index, const LexedDateTimePart &part)
{
    bool result = true;

    if (part.length != 2)
    {
        minute_wrong_length(lexer, part.source);
        result = false;
    }
    else if (value[index] > '5')
    {
        minute_out_of_range(lexer, part.source);
        result = false;
    }

    return result;
}


bool
validate_hour(Lexer &lexer, const string &value, u64 index, const LexedDateTimePart &part)
{
    bool result = true;

    if (part.length != 2)
    {
        hour_wrong_length(lexer, part.source);
        result = false;
    }
    else if ((value[index] > '2') || ((value[index] == '2') && (value[index + 1] > '3')))
    {
        hour_out_of_range(lexer, part.source);
        result = false;
    }

    return result;
}


bool
validate_timezone(Lexer &lexer, const LexedDateTime &datetime)
{
    bool result = true;

    if (datetime.type == Value::Type::OFFSET_DATETIME)
    {
        // offset datetime format
        // 0         1         2         3
        // 01234567890123456789012345678901
        // YYYY-MM-DD HH:MM:SS.SSSSSS+HH:MM
        u64 index = 27;
        result = validate_hour(lexer, datetime.value, index, datetime.tz_hour)
            && validate_minute(lexer, datetime.value, index + 3, datetime.tz_minute);
    }

    return result;
}


bool
validate_time(Lexer &lexer, const LexedDateTime &datetime)
{
    bool result = true;

    if (datetime.type != Value::Type::LOCAL_DATE)
    {
        // datetime format
        // 0         1         2
        // 01234567890123456789012345
        // YYYY-MM-DD HH:MM:SS.SSSSSS
        u64 index = datetime.type == Value::Type::LOCAL_TIME ? 0 : 11;
        result = validate_hour(lexer, datetime.value, index, datetime.hour)
            && validate_minute(lexer, datetime.value, index + 3, datetime.minute)
            && validate_second(lexer, datetime.value, index + 6, datetime.second);
        // microseconds don't need be validated: any numerical value is valid
        // and they're normalized to 6 digits when parsing
    }

    return result;
}


bool
validate_date(Lexer &lexer, const LexedDateTime &datetime)
{
    bool result = true;

    if (datetime.type != Value::Type::LOCAL_TIME)
    {
        if (datetime.year.length != 4)
        {
            year_wrong_length(lexer, datetime.year.source);
            result = false;
        }
        else if (datetime.month.length != 2)
        {
            month_wrong_length(lexer, datetime.month.source);
            result = false;
        }
        else if (datetime.day.length != 2)
        {
            day_wrong_length(lexer, datetime.day.source);
            result = false;
        }
        else
        {
            // date format
            // 0123456789
            // YYYY-MM-DD
            const char *value = datetime.value.c_str();
            int year = atoi(value);
            int month = atoi(value + 5);
            int day = atoi(value + 8);

            if ((month < 1) || (month > 12))
            {
                month_out_of_range(lexer, datetime.month.source);
                result = false;
            }
            else
            {
                constexpr int days_in_months[] = {
                    31, // January
                    28, // February
                    31, // March
                    30, // April
                    31, // May
                    30, // June
                    31, // July
                    31, // August
                    30, // September
                    31, // October
                    30, // November
                    31, // December
                };

                int days_in_month = days_in_months[month - 1];
                // leap years
                if ((month == 2) && ((year % 4) == 0) && (((year % 100) != 0) || (year % 400) == 0))
                {
                    days_in_month = 29;
                }

                if ((day < 1) || (day > days_in_month))
                {
                    day_out_of_range(lexer, datetime.day.source, days_in_month);
                    result = false;
                }
            }
        }
    }

    return result;
}


Token
validate_datetime(Lexer &lexer, const LexedDateTime &datetime, u32 context)
{
    Token result;
    if (datetime.type == Value::Type::INVALID)
    {
        invalid_value(lexer, context);
        result = make_value(lexer, Value());
    }
    else
    {
        if (validate_date(lexer, datetime)
            && validate_time(lexer, datetime)
            && validate_timezone(lexer, datetime))
        {
            switch (datetime.type)
            {
                case Value::Type::LOCAL_DATE:
                {
                    istringstream in(datetime.value);
                    LocalDate value;
                    in >> date::parse("%Y-%m-%d", value);
                    result = make_value(lexer, Value(value));
                } break;

                case Value::Type::LOCAL_DATETIME:
                {
                    istringstream in(datetime.value);
                    LocalDateTime value;
                    in >> date::parse("%Y-%m-%d %T", value);
                    result = make_value(lexer, Value(value));
                } break;

                case Value::Type::OFFSET_DATETIME:
                {
                    istringstream in(datetime.value);
                    OffsetDateTime value;
                    in >> date::parse("%Y-%m-%d %T%z", value);
                    result = make_value(lexer, Value(value));
                } break;


                case Value::Type::LOCAL_TIME:
                {
                    // time format
                    // 0         1
                    // 012345678901234
                    // HH:MM:SS.SSSSSS
                    const char *localtime = datetime.value.c_str();
                    int hour = atoi(localtime);
                    int minute = atoi(localtime + 3);
                    int second = atoi(localtime + 6);
                    int microsecond = atoi(localtime + 9);

                    LocalTime value(
                        chrono::hours(hour)
                        + chrono::minutes(minute)
                        + chrono::seconds(second) + chrono::microseconds(microsecond));
                    result = make_value(lexer, Value(value));
                } break;

                default:
                {
                    assert(false);
                } break;
            }
        }
        else
        {
            result = make_value(lexer, Value());
        }
    }

    return result;
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
            case '#':
            {
                eating = false;
            } break;

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
        invalid_value(lexer, context);
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
    // TODO stop at first invalid character instead of lexing as much as possible?
    // As a test case, consider the invalid (datetime) value: 1987-07-0517:45:00Z
    // This is missing a separator between the date and time, and since a ':'
    // is only valid within the context of a time (and we'd currently be lexing
    // a date), we eat the remainder of the value and return an invalid result
    // (emitting the rather generic error "invalid value"). But if we instead
    // stopped at the ':' and indicated the date as "done", we could instead
    // emit two errors: the first that the day should be 2 digits (since we'll
    // have lexed '0517'), and the second that we expected something else
    // (e.g., end-of-line) but instead found the remainder of the invalid
    // value. Despite (or because of) the increased verbosity, this could give
    // a more user-friendly indication that the problem is a missing separator.
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


void
lex_timezone(Lexer &lexer, LexedDateTime &datetime, LexedDigits &lexed, u32 context)
{
    assert(datetime.type == Value::Type::OFFSET_DATETIME);
    assert(lexed.result == LexedDigits::TIMEZONE);
    assert(match(lexer, '-') || match(lexer, '+'));
    datetime.value.push_back(eat(lexer));

    lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
    if (lexed.result == LexedDigits::TIME)
    {
        datetime.tz_hour.length = lexed.digits.length();
        datetime.tz_hour.source = lexed.start;
        datetime.value += move(lexed.digits);
    }
    else
    {
        datetime.type = Value::Type::INVALID;
    }

    if (datetime.type == Value::Type::OFFSET_DATETIME)
    {
        assert(match(lexer, ':'));
        datetime.value.push_back(eat(lexer));

        lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
        if (lexed.result == LexedDigits::DONE)
        {
            datetime.tz_minute.length = lexed.digits.length();
            datetime.tz_minute.source = lexed.start;
            datetime.value += move(lexed.digits);
        }
        else
        {
            datetime.type = Value::Type::INVALID;
        }
    }
}


void
lex_time(Lexer &lexer, LexedDateTime &datetime, LexedDigits &lexed, u32 context)
{
    assert(lexed.result == LexedDigits::TIME);
    assert((datetime.type == Value::Type::LOCAL_DATETIME) || (datetime.type == Value::Type::LOCAL_TIME));

    if ((datetime.type == Value::Type::LOCAL_DATETIME) || (datetime.value.length() == 0))
    {
        datetime.hour.length = lexed.digits.length();
        datetime.hour.source = lexed.start;
        datetime.value += move(lexed.digits);
    }
    else
    {
        datetime.type = Value::Type::INVALID;
    }

    if (datetime.type != Value::Type::INVALID)
    {
        assert(match(lexer, ':'));
        datetime.value.push_back(eat(lexer));

        lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
        if (lexed.result == LexedDigits::TIME)
        {
            datetime.minute.length = lexed.digits.length();
            datetime.minute.source = lexed.start;
            datetime.value += move(lexed.digits);
        }
        else
        {
            datetime.type = Value::Type::INVALID;
        }
    }

    if (datetime.type != Value::Type::INVALID)
    {
        assert(match(lexer, ':'));
        datetime.value.push_back(eat(lexer));

        lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
        switch (lexed.result)
        {
            case LexedDigits::DONE:
            {
                datetime.second.length = lexed.digits.length();
                datetime.second.source = lexed.start;
                datetime.value += move(lexed.digits);

                datetime.value += ".000000";
                datetime.microsecond.length = 6;
                datetime.microsecond.source = lexer.current;
            }  break;

            case LexedDigits::FRACTION:
            {
                datetime.second.length = lexed.digits.length();
                datetime.second.source = lexed.start;
                datetime.value += move(lexed.digits);
            } break;

            case LexedDigits::TIMEZONE:
            case LexedDigits::ZULU:
            {
                if (datetime.type == Value::Type::LOCAL_DATETIME)
                {
                    datetime.type = Value::Type::OFFSET_DATETIME;
                    datetime.second.length = lexed.digits.length();
                    datetime.second.source = lexed.start;
                    datetime.value += move(lexed.digits);

                    datetime.value += ".000000";
                    datetime.microsecond.length = 6;
                    datetime.microsecond.source = lexer.current;
                }
                else
                {
                    datetime.type = Value::Type::INVALID;
                }
            } break;

            default:
            {
                datetime.type = Value::Type::INVALID;
            } break;
        }
    }

    if ((datetime.type != Value::Type::INVALID) && (lexed.result == LexedDigits::FRACTION))
    {
        assert(match(lexer, '.'));
        datetime.value.push_back(eat(lexer));

        lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);
        switch (lexed.result)
        {
            case LexedDigits::DONE:
            {
                lexed.digits.resize(6, '0');
                datetime.microsecond.length = lexed.digits.length();
                datetime.microsecond.source = lexed.start;
                datetime.value += move(lexed.digits);
            } break;

            case LexedDigits::TIMEZONE:
            case LexedDigits::ZULU:
            {
                if (datetime.type == Value::Type::LOCAL_DATETIME)
                {
                    datetime.type = Value::Type::OFFSET_DATETIME;
                    lexed.digits.resize(6, '0');
                    datetime.microsecond.length = lexed.digits.length();
                    datetime.microsecond.source = lexed.start;
                    datetime.value += move(lexed.digits);
                }
                else
                {
                    datetime.type = Value::Type::INVALID;
                }
            } break;

            default:
            {
                datetime.type = Value::Type::INVALID;
            }
        }
    }
}


void
lex_date(Lexer &lexer, LexedDateTime &datetime, LexedDigits &lexed, u32 context)
{
    assert(lexed.result == LexedDigits::DATE);

    if (datetime.value.length() == 0)
    {
        datetime.year.length = lexed.digits.length();
        datetime.year.source = lexed.start;
        datetime.value += move(lexed.digits);
    }
    else
    {
        datetime.type = Value::Type::INVALID;
    }

    if (datetime.type == Value::Type::LOCAL_DATE)
    {
        assert(match(lexer, '-'));
        datetime.value.push_back(eat(lexer));

        lexed = lex_digits(lexer, is_decimal, context | LEX_DATE);
        if (lexed.result == LexedDigits::DATE)
        {
            datetime.month.length = lexed.digits.length();
            datetime.month.source = lexed.start;
            datetime.value += move(lexed.digits);
        }
        else
        {
            datetime.type = Value::Type::INVALID;
        }
    }

    if (datetime.type == Value::Type::LOCAL_DATE)
    {
        assert(match(lexer, '-'));
        datetime.value.push_back(eat(lexer));

        lexed = lex_digits(lexer, is_decimal, context | LEX_DATE);
        switch (lexed.result)
        {
            case LexedDigits::DONE:
            {
                if (match(lexer, ' ') && is_decimal(peek(lexer, 1)))
                {
                    datetime.type = Value::Type::LOCAL_DATETIME;
                }
                datetime.day.length = lexed.digits.length();
                datetime.day.source = lexed.start;
                datetime.value += move(lexed.digits);
            } break;

            case LexedDigits::TIME:
            {
                datetime.type = Value::Type::LOCAL_DATETIME;
                datetime.day.length = lexed.digits.length();
                datetime.day.source = lexed.start;
                datetime.value += move(lexed.digits);
            } break;

            default:
            {
                datetime.type = Value::Type::INVALID;
            } break;
        }
    }

    if (datetime.type == Value::Type::LOCAL_DATETIME)
    {
        assert(match(lexer, 'T') || match(lexer, 't') || match(lexer, ' '));
        advance(lexer);
        datetime.value.push_back(' ');

        lexed = lex_digits(lexer, is_decimal, context | LEX_TIME);

        if (lexed.result == LexedDigits::TIME)
        {
            lex_time(lexer, datetime, lexed, context);
        }
        else
        {
            datetime.type = Value::Type::INVALID;
        }
    }

    if (datetime.type == Value::Type::OFFSET_DATETIME)
    {
        if (lexed.result == LexedDigits::ZULU)
        {
            // synthesize an offset timezone
            datetime.value += "+00:00";
            datetime.tz_hour.length = 2;
            datetime.tz_hour.source = lexer.current;
            datetime.tz_minute.length = 2;
            datetime.tz_hour.source = lexer.current;

            // eat the 'Z'
            advance(lexer);
        }
        else
        {
            assert(lexed.result == LexedDigits::TIMEZONE);
            datetime.type = Value::Type::OFFSET_DATETIME;
            lex_timezone(lexer, datetime, lexed, context);
        }
    }
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
        invalid_value(lexer, context);
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
            invalid_value(lexer, context);
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
            invalid_value(lexer, context);
            result = make_value(lexer, Value());
        } break;
    }

    if ((result.value.type() != Value::Type::INVALID)
        && (integer.digits[0] == '0') && (integer.digits.length() > 1))
    {
        leading_zeros(lexer, integer.start);
    }

    return result;
}


Token
lex_decimal(Lexer &lexer, string &value, u32 context)
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
            LexedDateTime datetime(value, Value::Type::LOCAL_DATE);
            lex_date(lexer, datetime, lexed, context);
            result = validate_datetime(lexer, datetime, context);
        } break;

        case LexedDigits::TIME:
        {
            LexedDateTime datetime(value, Value::Type::LOCAL_TIME);
            lex_time(lexer, datetime, lexed, context);
            result = validate_datetime(lexer, datetime, context);
        } break;

        default:
        {
            invalid_value(lexer, context);
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
            unallowed_sign(lexer);
        }
        result = make_value(lexer, Value(string_to_s64(lexed.digits, 16)));
    }
    else
    {
        invalid_hexadecimal(lexer, context);
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
            unallowed_sign(lexer);
        }
        result = make_value(lexer, Value(string_to_s64(lexed.digits, 8)));
    }
    else
    {
        invalid_octal(lexer, context);
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
            unallowed_sign(lexer);
        }
        result = make_value(lexer, Value(string_to_s64(lexed.digits, 2)));
    }
    else
    {
        invalid_binary(lexer, context);
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
            result = lex_decimal(lexer, value, context);
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
        result = lex_decimal(lexer, value, context);
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
        invalid_utf8_byte(lexer, location, c);
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
            incomplete_unicode_codepoint(lexer, location, nbytes, eaten);
            valid = false;
        }
    }

    if (valid)
    {
        if (eaten == nbytes)
        {
            if (codepoint <= 0x7f)
            {
                nbytes = 1;
            }
            else if (codepoint <= 0x7ff)
            {
                nbytes = 2;
            }
            else if (codepoint <= 0xffff)
            {
                nbytes = 3;
            }
            else
            {
                nbytes = 4;
            }

            if (eaten > nbytes)
            {
                overlong_utf8_encoding(lexer, location, codepoint, nbytes, eaten);
                valid = false;
            }
            else if (codepoint > 0x10ffff)
            {
                invalid_unicode_codepoint(lexer, location, codepoint);
                valid = false;
            }
            else if (!(
                (codepoint == 0x09)
                || ((codepoint >= 0x20) && (codepoint <= 0x7e))
                || ((codepoint >= 0x80) && (codepoint <= 0xd7ff))
                || (codepoint >= 0xe000)))
            {
                unallowed_unicode_codepoint(lexer, location, codepoint);
                valid = false;
            }
        }
        else
        {
            incomplete_unicode_codepoint(lexer, location, nbytes, eaten);
            valid = false;
        }
    }

    // Adjust the current column to reflect only 1 "character".
    // TODO Actually do this correctly (e.g., combining characters)
    lexer.current.column -= (eaten - 1);

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

    // If we've hit the end of the file, then we'll error on an unclosed string
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
            unclosed_string(lexer);
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
            unclosed_string(lexer);
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
                invalid_value(lexer, context);
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
        invalid_key(lexer, value);
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
                        unallowed_unicode_codepoint(lexer, lexer.current, b);
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


bool
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

    return valid;
}


} // namespace toml
