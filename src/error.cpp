#include "error.hpp"

#include <iomanip>
#include <sstream>


using namespace std;


namespace toml
{


namespace
{


constexpr char HEX2CHAR[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                              'A', 'B', 'C', 'D', 'E', 'F' };


void
add_error(vector<Error> &errors, const SourceLocation &location, string &&message)
{
    auto insert_at = errors.end();
    for ( ; insert_at != errors.begin(); --insert_at)
    {
        // vector::end() is not a valid index and vector::insert() inserts
        // _before_ the iterator, so we need to check against the element prior
        // to the current index of the iterator
        auto check = insert_at - 1;
        if (check->location.index <= location.index)
        {
            break;
        }
    }

    errors.insert(insert_at, { location, move(message) });
}


void
add_error(Lexer &lexer, string &&message)
{
    add_error(lexer.errors, lexer.start, move(message));
}


void
add_error(Lexer &lexer, const SourceLocation &location, string &&message)
{
    add_error(lexer.errors, location, move(message));
}


void
add_error(Parser &parser, string &&message)
{
    add_error(parser.errors, parser.token.location, move(message));
}


string
get_lexeme(Parser &parser)
{
    string result = get_lexeme(parser.lexer, parser.token.location.index);
    return result;
}


string
byte_to_hex(byte value)
{
    string result = "0x";
    result.push_back(HEX2CHAR[(value >> 4) & 0xf]);
    result.push_back(HEX2CHAR[value & 0xf]);
    return result;
}


string
format_unicode(u32 codepoint)
{
    ostringstream o;
    o << "U+" << setw(4) << setfill('0') << uppercase << hex << codepoint;

    string result = o.str();
    return result;
}


} // namespace


void
expected_end_of_line(Parser &parser)
{
    constexpr u32 context = LEX_EOL;

    if (resynchronize(parser.lexer, context))
    {
        string message = "Expected the end of the line but got: " + get_lexeme(parser);
        add_error(parser, move(message));
    }
    advance(parser, context);
}


void
incomplete_unicode_codepoint(Lexer &lexer, const SourceLocation &location, s32 expected, s32 actual)
{
    string message = "Invalid UTF-8: expected a " + to_string(expected) + "-byte Unicode codepoint but encountered invalid or missing bytes after decoding " + to_string(actual) + " byte(s).";
    add_error(lexer, location, move(message));
}


void
invalid_utf8_byte(Lexer &lexer, const SourceLocation &location, byte b)
{
    string message = "Invalid UTF-8 byte: " + byte_to_hex(b);
    add_error(lexer, location, move(message));
}


void
invalid_unicode_codepoint(Lexer &lexer, const SourceLocation &location, u32 codepoint)
{
    string message = "Invalid Unicode codepoint: " + format_unicode(codepoint);
    add_error(lexer, location, move(message));
}


void
invalid_unicode_escape(Lexer &lexer, const SourceLocation &location, u32 expected, u32 actual)
{
    string message = "Invalid or incomplete Unicode escape sequence: expected " + to_string(expected) + " hexadecimal characters but found " + to_string(actual) + ".";
    add_error(lexer, location, move(message));
}


void
overlong_utf8_encoding(Lexer &lexer, const SourceLocation &location, u32 codepoint, u32 expected, u32 actual)
{
    string message = "Invalid UTF-8: Unicode codepoint " + format_unicode(codepoint) + " should be " + to_string(expected) + " bytes but was in encoded using " + to_string(actual) + " bytes.";
    add_error(lexer, location, move(message));
}


void
day_out_of_range(Lexer &lexer, const SourceLocation &location, s32 days_in_month)
{
    add_error(lexer, location, "Day must be between 01 and " + to_string(days_in_month) + " inclusive.");
}


void
year_wrong_length(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Year must be four digits");
}


void
month_wrong_length(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Month must be two digits.");
}


void
day_wrong_length(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Day must be two digits.");
}


void
hour_wrong_length(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Hour must be two digits.");
}


void
minute_wrong_length(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Minute must be two digits.");
}


void
second_wrong_length(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Second must be two digits.");
}


void
month_out_of_range(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Month must be between 01 and 12 inclusive.");
}


void
hour_out_of_range(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Hour must be between 00 and 23 inclusive.");
}


void
minute_out_of_range(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Minute must be between 00 and 59 inclusive.");
}


void
second_out_of_range(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Second must be between 00 and 59 inclusive.");
}


void
invalid_escape(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Invalid escape sequence: " + get_lexeme(lexer, location.index));
}


void
invalid_expression(Parser &parser, u32 context)
{
    if (resynchronize(parser.lexer, context))
    {
        add_error(parser, "Invalid expression: " + get_lexeme(parser));
    }
    advance(parser, context);
}


void
invalid_key(Lexer &lexer, const string &key)
{
    add_error(lexer, "Invalid key: " + key);
}


void
invalid_key(Parser &parser, u32 context)
{
    if (resynchronize(parser.lexer, context))
    {
        add_error(parser, "Invalid key: " + get_lexeme(parser));
    }
    advance(parser, context);
}


void
invalid_binary(Lexer &lexer, u32 context)
{
    resynchronize(lexer, context);
    add_error(lexer, "Invalid binary number: " + get_lexeme(lexer));
}


void
invalid_hexadecimal(Lexer &lexer, u32 context)
{
    resynchronize(lexer, context);
    add_error(lexer, "Invalid hexadecimal number: " + get_lexeme(lexer));
}


void
invalid_octal(Lexer &lexer, u32 context)
{
    resynchronize(lexer, context);
    add_error(lexer, "Invalid octal number: " + get_lexeme(lexer));
}


void
invalid_value(Lexer &lexer, u32 context)
{
    if (resynchronize(lexer, context))
    {
        add_error(lexer, "Invalid value: " + get_lexeme(lexer));
    }
}


void
invalid_value(Parser &parser, u32 context)
{
    if (resynchronize(parser.lexer, context))
    {
        add_error(parser, "Invalid value: " + get_lexeme(parser));
    }
    advance(parser, context);
}


void
key_redefinition(Parser &parser, const Key &key, const Key &prev)
{
    string message = "Key '" + key.value + "' has already been defined on line " + to_string(prev.location.line) + ", character " + to_string(prev.location.column) + ".";
    add_error(parser.errors, key.location, move(message));
}


void
leading_zeros(Lexer &lexer, const SourceLocation &location)
{
    add_error(lexer, location, "Number may not have leading zeros.");
}


void
missing_array_separator(Parser &parser)
{
    add_error(parser, "Missing ',' between array values.");
}


void
missing_inline_table_separator(Parser &parser)
{
    add_error(parser, "Missing ',' between key-value pairs.");
}


void
missing_keyval(Parser &parser)
{
    add_error(parser, "Expected a key-value pair.");
}


void
missing_keyval_separator(Parser &parser)
{
    add_error(parser, "Missing '=' between key and value.");
}


void
missing_value(Parser &parser)
{
    add_error(parser, "Expected a value.");
}


void
unallowed_sign(Lexer &lexer)
{
    add_error(lexer, "A leading sign is not allowed.");
}


void
unallowed_unicode_codepoint(Lexer &lexer, const SourceLocation &location, u32 codepoint)
{
    string message = "Unicode codepoint " + format_unicode(codepoint) + " is not allowed.";
    add_error(lexer, location, move(message));
}


void
unclosed_array(Parser &parser)
{
    add_error(parser, "Missing closing ']' for array.");
}


void
unclosed_inline_table(Parser& parser)
{
    add_error(parser, "Missing closing '}' for inline table.");
}


void
unclosed_string(Lexer &lexer)
{
    add_error(lexer, lexer.current, "Unclosed string.");
}


void
unclosed_table_header(Parser &parser, u32 context)
{
    switch (parser.token.type)
    {
        case TOKEN_COMMENT:
        case TOKEN_NEWLINE:
        case TOKEN_EOF:
        {
            add_error(parser, "Missing closing ']' for table header.");
        } break;

        default:
        {
            if (resynchronize(parser.lexer, context))
            {
            add_error(parser, "Expected closing ']' for table header.");
            }
            advance(parser, context);
        } break;
    }
}


} // namespace toml
