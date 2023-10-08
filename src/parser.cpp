//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include "parser.hpp"

#include "common.hpp"
#include "error.hpp"


using namespace std;


namespace toml
{


namespace {

// Keys can only be defined once. Although this seems like a fairly
// straightforward statement, things can get confusing with tables and dotted
// keys. As of TOML 1.0.0, the rules seems to be:
//
// Inline tables, and everything in them, are defined and never updatable.
// Inline tables could be considered "table literals". (When considering table
// arrays below, one might similarly consider "regular" arrays to be "inline
// arrays" or "array literals" and non-updatable.)
//
// Every dot in a dotted key in a key-value pair defines the table that
// precedes the dot. A table defined by a dotted key can only be updated while
// in the context of the table header in which it was initially defined (or, if
// no table header has been specified, while in the context of the implicit
// root table).
//
// Table headers only define the last table name that appears in the header. If
// there are no dots in the header name, this is simply the specified name,
// otherwise it's the name after the last dot. Any names before a dot are
// implicitly created as tables (if they don't already exist) but DO NOT define
// them. Because they don't count as a "definition", dotted headers can
// update/extend already-defined tables (except inline tables). (Somewhat
// confusingly, although table headers can be dotted and effectively just add
// keys to the parent table -- the specification even refers the reader to the
// section on keys to understand their behavior -- the term "dotted keys" seems
// to only apply to dotted keys in a key-value pair.)
//
// Because dotted keys DO define tables while dotted headers DON'T, it's not
// possible to try to update a dotted key table by defining it in a dotted
// header and then trying to update it from a containing header, because the
// dotted key has to "pass through" the already-defined table causing a
// redefinition error:
//     [one.two]        # table 'one' implicitly created, table 'two' defined
//     three.a = 'foo'  # table 'three' defined
//
//     [one]                # table 'one' now defined
//     two.three.b = 'bar'  # ERROR: table 'two' is redefined
//
// The same is true in the reverse:
//     [one]                # table 'one' is defined
//     two.three.a = 'foo'  # tables 'two' and 'three' are defined
//
//     [one.two]        # ERROR: table 'two' is redefined
//     three.b = 'bar'
//
// For parsing, this means that if we are in a dotted table context and are
// updating a dotted table, we can always be certain that either 1) we are
// doing so validly, or 2) we've already emitted a redefinition error.
//
// Table array headers behave the same as table headers except, instead of
// defining a table, they initially define an array and then define and append
// a table to the array. Dotted table headers and dotted table array headers
// work with each other: if a dotted name refers to a table array (but NOT a
// "regular" array), then the last table in the array becomes the target of
// subsequent updates. This is true only for dotted headers, NOT dotted keys.
//
// Because of this implicit behavior, it is possible to get conflicting
// definitions even though, theoretically, nothing has been defined:
//     [one.two]    # 'one' is implicitly created as a table
//     [[one]]      # ERROR: 'one' cannot now be defined as an array
//
// The reverse case is fine though:
//     [[one]]      # 'one' is defined as a table array. A table is also
//                  # defined and appended to it
//     [one.two]    # table 'two' is defined as part of the last table in 'one'
//
// In order to enforce these rules, some meta data is needed to keep track of
// the context in which tables are created:



//
// Predeclarations
//

void parse_keyval(Parser &parser, DefinitionTable *table, TableMeta *meta, u32 context);

Definition parse_value(Parser &parser, u32 context);



//
// Implementation
//

void
add_error(Parser &parser, string message)
{
    Error error = {parser.token.location, move(message)};

    auto insert_at = parser.errors.end();
    for ( ; insert_at != parser.errors.begin(); --insert_at)
    {
        auto check = insert_at - 1;
        if (check->location.index <= error.location.index)
        {
            break;
        }
    }

    parser.errors.insert(insert_at, move(error));
}


string
resynchronize(Parser &parser, string message, u32 context)
{
    resynchronize(parser.lexer, context);
    string result = get_lexeme(parser.lexer, parser.token.location.index);

    if (message.length())
    {
        add_error(parser, message + result);
    }

    advance(parser, context);
    return result;
}


Token
eat(Parser &parser, u32 context)
{
    Token result = parser.token;
    advance(parser, context);
    return result;
}


Token&
peek(Parser &parser)
{
    return parser.token;
}


bool
peek(Parser &parser, TokenType expected)
{
    bool result = parser.token.type == expected;
    return result;
}


Definition
parse_array(Parser &parser, u32 context)
{
    assert(peek(parser, TOKEN_LBRACKET));

    TokenType prev = TOKEN_LBRACKET;
    u32 array_context = context | LEX_ARRAY | LEX_VALUE;
    Definition result(Value::Type::ARRAY, eat(parser, array_context).location);

    bool parsing = true;
    while (parsing)
    {
        Token token = peek(parser);
        switch (token.type)
        {
            case TOKEN_COMMA:
            {
                if (prev != TOKEN_VALUE)
                {
                    missing_value(parser);
                }
                array_context = context | LEX_ARRAY | LEX_VALUE;
                advance(parser, array_context);
                prev = TOKEN_COMMA;
            } break;

            case TOKEN_RBRACKET:
            {
                advance(parser, context);
                parsing = false;
            } break;

            case TOKEN_COMMENT:
            case TOKEN_NEWLINE:
            {
                advance(parser, array_context);
            } break;

            case TOKEN_EOF:
            {
                unclosed_array(parser);
                parsing = false;
            } break;

            default:
            {
                if (prev == TOKEN_VALUE)
                {
                    missing_array_separator(parser);
                }
                if (token.type == TOKEN_NONE)
                {
                    advance(parser, context | LEX_ARRAY | LEX_VALUE);
                }

                array_context = context | LEX_ARRAY;
                Definition value = parse_value(parser, array_context);
                result.as_array().push_back(move(value));
                prev = TOKEN_VALUE;
            } break;
        }
    }

    return result;
}


Definition
parse_inline_table(Parser &parser, u32 context)
{
    assert(peek(parser, TOKEN_LBRACE));

    TokenType prev = TOKEN_LBRACE;
    u32 table_context = context | LEX_TABLE | LEX_KEY;
    Definition result(Value::Type::TABLE, eat(parser, table_context).location);
    TableMeta meta;

    bool parsing = true;
    while (parsing)
    {
        Token token = peek(parser);
        switch (token.type)
        {
            case TOKEN_COMMA:
            {
                if (prev != TOKEN_VALUE)
                {
                    missing_keyval(parser);
                }
                table_context = context | LEX_TABLE | LEX_KEY;
                advance(parser, table_context);
                prev = TOKEN_COMMA;
            } break;

            case TOKEN_RBRACE:
            {
                if (prev == TOKEN_COMMA)
                {
                    missing_keyval(parser);
                }
                advance(parser, context);
                parsing = false;
            } break;

            case TOKEN_COMMENT:
            case TOKEN_EOF:
            case TOKEN_NEWLINE:
            {
                if (prev == TOKEN_COMMA)
                {
                    missing_keyval(parser);
                }
                unclosed_inline_table(parser);
                parsing = false;
            } break;

            default:
            {
                if (prev == TOKEN_VALUE)
                {
                    missing_inline_table_separator(parser);
                }
                if (token.type == TOKEN_NONE)
                {
                    advance(parser, context | LEX_TABLE | LEX_KEY);
                }

                table_context = context | LEX_TABLE;
                parse_keyval(parser, &result.as_table(), &meta, table_context);
                prev = TOKEN_VALUE;
            } break;
        }
    }

    return result;
}


Definition
parse_value(Parser &parser, u32 context)
{
    Definition result;

    Token token = peek(parser);
    switch(token.type)
    {
        case TOKEN_VALUE:
        {
            result = Definition(token.value, token.location);
            advance(parser, context);
        } break;

        case TOKEN_LBRACKET:
        {
            result = parse_array(parser, context);
        } break;

        case TOKEN_LBRACE:
        {
            result = parse_inline_table(parser, context);
        } break;

        case TOKEN_COMMENT:
        case TOKEN_NEWLINE:
        case TOKEN_EOF:
        {
            missing_value(parser);
        } break;

        case TOKEN_RBRACE:
        {
            if (context & LEX_TABLE)
            {
                missing_value(parser);
            }
            else
            {
                invalid_value(parser, context);
            }
        } break;

        case TOKEN_RBRACKET:
        {
            if (context & LEX_ARRAY)
            {
                missing_value(parser);
            }
            else
            {
                invalid_value(parser, context);
            }
        } break;

        default:
        {
            invalid_value(parser, context);
        } break;
    }

    return result;
}


void
add_dotted_table(Parser &parser, DefinitionTable *&table, TableMeta *&meta, Token &token, bool valid)
{
    Key key = {token.lexeme, token.location};

    Definition &value = (*table)[key];
    ValueMeta &value_meta = (*meta)[key.value];
    if (value.type() == Value::Type::INVALID)
    {
        assert(value_meta.type() == ValueMeta::Type::INVALID);
        value = Definition(Value::Type::TABLE, key.location);
        value_meta = ValueMeta(ValueMeta::Type::DOTTED_TABLE);
    }
    else if (value_meta.type() == ValueMeta::Type::IMPLICIT_TABLE)
    {
        value_meta.make_table_dotted();
    }
    else if (value_meta.type() != ValueMeta::Type::DOTTED_TABLE)
    {
        if (valid)
        {
            key_redefinition(parser, key, table->find(key)->first);
        }
        // Replacing the value and continuing on seems like maybe the
        // best way to mitigate cascading errors.
        value = Definition(Value::Type::TABLE, key.location);
        value_meta = ValueMeta(ValueMeta::Type::DOTTED_TABLE);
    }

    assert(value.type() == Value::Type::TABLE);
    table = &value.as_table();

    assert(value_meta.type() == ValueMeta::Type::DOTTED_TABLE);
    meta = &value_meta.table();
}


void
parse_keyval(Parser &parser, DefinitionTable *table, TableMeta *meta, u32 context)
{
    Key key;
    bool valid = true;
    bool parsing = true;
    while (parsing)
    {
        Token token = peek(parser);
        switch (token.type)
        {
            case TOKEN_KEY:
            {
                advance(parser, context);
            } break;

            case TOKEN_COMMENT:
            case TOKEN_EOF:
            case TOKEN_EQUAL:
            case TOKEN_NEWLINE:
            case TOKEN_PERIOD:
            {
                add_error(parser, "Missing key.");
                valid = false;
            }  break;

            default:
            {
                if (((token.type == TOKEN_RBRACKET) && (context & LEX_ARRAY))
                    || ((token.type == TOKEN_RBRACE) && (context & LEX_TABLE)))
                {
                    add_error(parser, "Missing key.");
                }
                else
                {
                    invalid_key(parser, context | LEX_KEY);
                }
                valid = false;
            } break;
        }

        if (peek(parser, TOKEN_PERIOD))
        {
            add_dotted_table(parser, table, meta, token, valid);
            advance(parser, context | LEX_KEY);
        }
        else
        {
            key = {token.lexeme, token.location};
            parsing = false;
        }
    }

    Definition &value = (*table)[key];
    ValueMeta &value_meta = (*meta)[key.value];
    if ((value.type() != Value::Type::INVALID) && valid)
    {
        key_redefinition(parser, key, table->find(key)->first);
    }

    if (peek(parser, TOKEN_EQUAL))
    {
        advance(parser, context | LEX_VALUE);
    }
    else
    {
        missing_keyval_separator(parser);
        if (peek(parser, TOKEN_NONE))
        {
            advance(parser, context | LEX_VALUE);
        }
    }

    value = parse_value(parser, context);
    value_meta = ValueMeta(ValueMeta::Type::LITERAL);
}


void
add_implicit_table(Parser &parser, Token &token, bool valid)
{
    Key key = {token.lexeme, token.location};

    Definition &value = (*parser.current_table)[key];
    ValueMeta &value_meta = (*parser.current_meta)[key.value];
    if (value.type() == Value::Type::INVALID)
    {
        assert(value_meta.type() == ValueMeta::Type::INVALID);
        value = Definition(Value::Type::TABLE, key.location);
        value_meta = ValueMeta(ValueMeta::Type::IMPLICIT_TABLE);

        parser.current_table = &value.as_table();
        parser.current_meta = &value_meta.table();
    }
    else
    {
        switch (value_meta.type())
        {
            case ValueMeta::Type::TABLE_ARRAY:
            {
                DefinitionArray &array = value.as_array();
                assert(array.size());

                ArrayMeta &array_meta = value_meta.array();
                assert(array_meta.back().type() == ValueMeta::Type::HEADER_TABLE);

                parser.current_table = &array.back().as_table();
                parser.current_meta = &array_meta.back().table();
            } break;

            case ValueMeta::Type::IMPLICIT_TABLE:
            case ValueMeta::Type::DOTTED_TABLE:
            case ValueMeta::Type::HEADER_TABLE:
            {
                assert(value.type() == Value::Type::TABLE);
                parser.current_table = &value.as_table();
                parser.current_meta = &value_meta.table();
            } break;

            default:
            {
                if (valid)
                {
                    key_redefinition(parser, key, parser.current_table->find(key)->first);
                }
                // Replacing the value and continuing on seems like maybe the
                // best way to mitigate cascading errors.
                value = Definition(Value::Type::TABLE, key.location);
                value_meta = ValueMeta(ValueMeta::Type::IMPLICIT_TABLE);

                parser.current_table = &value.as_table();
                parser.current_meta = &value_meta.table();
            } break;
        }
    }
}


Key
parse_table_header(Parser &parser, u32 context)
{
    Key result;
    bool valid = true;
    bool parsing = true;
    while (parsing)
    {
        Token token = peek(parser);
        switch (token.type)
        {
            case TOKEN_KEY:
            {
                advance(parser, context);
            } break;

            case TOKEN_COMMENT:
            case TOKEN_EOF:
            case TOKEN_NEWLINE:
            case TOKEN_PERIOD:
            case TOKEN_RBRACKET:
            case TOKEN_DOUBLE_RBRACKET:
            {
                add_error(parser, "Missing key.");
                valid = false;
            }  break;

            default:
            {
                invalid_key(parser, context);
                valid = false;
            } break;
        }

        if (peek(parser, TOKEN_PERIOD))
        {
            add_implicit_table(parser, token, valid);
            advance(parser, context);
        }
        else
        {
            result = {token.lexeme, token.location};
            parsing = false;
        }
    }

    return result;
}


void
parse_table(Parser &parser)
{
    assert(peek(parser, TOKEN_LBRACKET));

    parser.current_table = &parser.table;
    parser.current_meta = &parser.meta;

    advance(parser, LEX_TABLE_HEADER);
    Key key = parse_table_header(parser, LEX_TABLE_HEADER);

    Definition &value = (*parser.current_table)[key];
    ValueMeta &value_meta = (*parser.current_meta)[key.value];
    if (value.type() == Value::Type::INVALID)
    {
        assert(value_meta.type() == ValueMeta::Type::INVALID);
        value = Definition(Value::Type::TABLE, key.location);
        value_meta = ValueMeta(ValueMeta::Type::HEADER_TABLE);
    }
    else if (value_meta.type() == ValueMeta::Type::IMPLICIT_TABLE)
    {
        value_meta.make_table_header();
    }
    else
    {
        key_redefinition(parser, key, parser.current_table->find(key)->first);
        // Replacing the value and continuing on seems like maybe the best way
        // to mitigate cascading errors.
        value = Definition(Value::Type::TABLE, key.location);
        value_meta = ValueMeta(ValueMeta::Type::HEADER_TABLE);
    }

    assert(value.type() == Value::Type::TABLE);
    assert(value_meta.type() == ValueMeta::Type::HEADER_TABLE);
    parser.current_table = &value.as_table();
    parser.current_meta = &value_meta.table();

    Token token = peek(parser);
    switch (token.type)
    {
        case TOKEN_RBRACKET:
        {
            advance(parser, LEX_EOL);
        } break;

        default:
        {
            unclosed_table_header(parser, LEX_EOL);
        }
    }
}


void
parse_table_array(Parser &parser)
{
    assert(peek(parser, TOKEN_DOUBLE_LBRACKET));

    parser.current_table = &parser.table;
    parser.current_meta = &parser.meta;

    advance(parser, LEX_ARRAY_HEADER);
    Key key = parse_table_header(parser, LEX_ARRAY_HEADER);

    Definition &value = (*parser.current_table)[key];
    ValueMeta &value_meta = (*parser.current_meta)[key.value];
    if (value.type() == Value::Type::INVALID)
    {
        assert(value_meta.type() == ValueMeta::Type::INVALID);
        value = Definition(Value::Type::ARRAY, key.location);
        value_meta = ValueMeta(ValueMeta::Type::TABLE_ARRAY);
    }
    else if (value_meta.type() != ValueMeta::Type::TABLE_ARRAY)
    {
        key_redefinition(parser, key, parser.current_table->find(key)->first);
        // Replacing the value and continuing on seems like maybe the best way
        // to mitigate cascading errors.
        value = Definition(Value::Type::ARRAY, key.location);
        value_meta = ValueMeta(ValueMeta::Type::TABLE_ARRAY);
    }

    assert(value.type() == Value::Type::ARRAY);
    DefinitionArray &array = value.as_array();
    array.push_back(Definition(Value::Type::TABLE, key.location));

    assert(value_meta.type() == ValueMeta::Type::TABLE_ARRAY);
    ArrayMeta &array_meta = value_meta.array();
    array_meta.push_back(ValueMeta(ValueMeta::Type::HEADER_TABLE));

    parser.current_table = &array.back().as_table();
    parser.current_meta = &array_meta.back().table();

    Token token = peek(parser);
    switch (token.type)
    {
        case TOKEN_DOUBLE_RBRACKET:
        {
            advance(parser, LEX_EOL);
        } break;

        case TOKEN_COMMENT:
        case TOKEN_EOF:
        case TOKEN_NEWLINE:
        {
            add_error(parser, "Missing closing ']]' for table array header.");
        } break;

        default:
        {
            add_error(parser, "Expected closing ']]' for table array header.");
            resynchronize(parser, "", LEX_EOL);
        }
    }
}


void
parse_expression(Parser &parser)
{
    Token token = peek(parser);
    switch (token.type)
    {
        case TOKEN_KEY:
        {
            parse_keyval(parser, parser.current_table, parser.current_meta, LEX_EOL);
        } break;

        case TOKEN_LBRACKET:
        {
            parse_table(parser);
        } break;

        case TOKEN_DOUBLE_LBRACKET:
        {
            parse_table_array(parser);
        } break;

        case TOKEN_COMMENT:
        case TOKEN_NEWLINE:
        case TOKEN_EOF:
        {
            // do nothing
        } break;

        // TODO special case matching on '.' and '=' to handle missing keys?
        default:
        {
            invalid_expression(parser, LEX_EOL);
        } break;
    }

    token = peek(parser);
    switch (token.type)
    {
        case TOKEN_COMMENT:
        {
            token = eat(parser, LEX_EOL);
        } // fallthrough

        case TOKEN_NEWLINE:
        case TOKEN_EOF:
        {
            // do nothing
        } break;

        default:
        {
            expected_end_of_line(parser);
        } break;
    }
}


Value
value_from_definition(Definition &definition)
{
    Value result;

    switch (definition.type())
    {
        case Value::Type::TABLE:
        {
            Value value(Table{});
            DefinitionTable &table = definition.as_table();
            for (auto keyval : table)
            {
                const Key &k = keyval.first;
                Definition d = keyval.second;
                value.as_table()[k.value] = value_from_definition(d);
            }

            result = value;
        } break;

        case Value::Type::ARRAY:
        {
            Value value(Array{});
            DefinitionArray &array = definition.as_array();
            for (auto r : array)
            {
                value.as_array().push_back(value_from_definition(r));
            }

            result = value;
        } break;

        default:
        {
            result = definition.as_value();
        } break;
    }

    return result;
}


} // namespace


void
advance(Parser &parser, u32 context)
{
    parser.token = next_token(parser.lexer, context);
}


bool
parse_with_metadata(const string &toml, DefinitionTable &definitions, vector<Error> &errors)
{
#if 0
    cout << "sizeof(string) = " << sizeof(string) << '\n';
    cout << "sizeof(integer) = " << sizeof(s64) << '\n';
    cout << "sizeof(float) = " << sizeof(f64) << '\n';
    cout << "sizeof(boolean) = " << sizeof(bool) << '\n';
    cout << "sizeof(OffsetDateTime) = " << sizeof(OffsetDateTime) << '\n';
    cout << "sizeof(LocalDateTime) = " << sizeof(LocalDateTime) << '\n';
    cout << "sizeof(LocalDate) = " << sizeof(LocalDate) << '\n';
    cout << "sizeof(LocalTime) = " << sizeof(LocalTime) << '\n';
    cout << "sizeof(Array) = " << sizeof(Array) << '\n';
    cout << "sizeof(Table) = " << sizeof(Table) << '\n';
    cout << "sizeof(void *) = " << sizeof(void *) << '\n';
    cout << "sizeof(unique_ptr<string>) = " << sizeof(unique_ptr<string>) << '\n';
    cout << "sizeof(unique_ptr<Array>) = " << sizeof(unique_ptr<Array>) << '\n';
    cout << "sizeof(unique_ptr<Table>) = " << sizeof(unique_ptr<Table>) << '\n';
    cout << "sizeof(Value) = " << sizeof(Value) << '\n';
    cout << "sizeof(Definition) = " << sizeof(Definition) << '\n';
#endif

    Lexer lexer(toml, errors);
    Parser parser(lexer, definitions, errors);

    for (advance(parser, LEX_KEY | LEX_TABLE_HEADER | LEX_ARRAY_HEADER);
         !peek(parser, TOKEN_EOF);
         advance(parser, LEX_KEY | LEX_TABLE_HEADER | LEX_ARRAY_HEADER))
    {
        parse_expression(parser);
    }

    bool result = errors.size() == 0;
    return result;
}


bool
parse_toml(const string &toml, Table &table, vector<Error> &errors)
{
    DefinitionTable definitions;
    bool result = parse_with_metadata(toml, definitions, errors);

    if (result)
    {
        for (auto keyval : definitions)
        {
            const Key &key = keyval.first;
            Definition definition = keyval.second;
            table[key.value] = value_from_definition(definition);
        }
    }

    return result;
}


} // namespace toml
