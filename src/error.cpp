#include "error.hpp"

#include <iomanip>
#include <sstream>


using namespace std;


namespace toml
{


namespace
{


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
        string message = "Expected the end of the line but got: " + get_lexeme(parser.lexer, parser.token.location.index);
        add_error(parser, move(message));
    }
    advance(parser, context);
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
        add_error(parser, "Invalid value: " + get_lexeme(parser.lexer, parser.token.location.index));
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
missing_array_separator(Parser &parser)
{
    add_error(parser, "Missing ',' between array values.");
}


void
missing_value(Parser &parser)
{
    add_error(parser, "Expected a value.");
}


void
unallowed_unicode_codepoint(Lexer &lexer, const SourceLocation &location, u32 codepoint)
{
    string message = "Unicode codepoint " + format_unicode(codepoint) + " is not allowed.";
    add_error(lexer, location, move(message));
}


void
unterminated_array(Parser &parser)
{
    add_error(parser, "Missing closing ']' for array.");
}


void
unterminated_inline_table(Parser& parser)
{
    add_error(parser, "Missing closing '}' for inline table.");
}


} // namespace toml
