#include "error.hpp"


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
add_error(Parser &parser, string &&message)
{
    add_error(parser.errors, parser.token.location, move(message));
}


} // namespace


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
        advance(parser, context);
    }
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
