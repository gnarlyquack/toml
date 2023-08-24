//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include "parser.hpp"

#include "common.hpp"
#include "lexer.hpp"


using namespace std;


namespace toml
{


using ErrorList = vector<Error>;
using TokenList = vector<Token>;


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

enum MetaType {
    META_TYPE_LITERAL,
    META_TYPE_IMPLICIT_TABLE,
    META_TYPE_DOTTED_TABLE,
    META_TYPE_HEADER_TABLE,
    META_TYPE_TABLE_ARRAY,
};


struct MetaValue final
{
    union
    {
        unordered_map<string, MetaValue *> *table;
        vector<MetaValue *> *array;
    };
    MetaType type;


    explicit MetaValue()
        : type(META_TYPE_LITERAL)
    {
    }


    explicit MetaValue(MetaType t)
        : type(t)
    {
        switch (type)
        {
            case META_TYPE_IMPLICIT_TABLE:
            case META_TYPE_DOTTED_TABLE:
            case META_TYPE_HEADER_TABLE:
            {
                table = new unordered_map<string, MetaValue *>{};
            } break;

            case META_TYPE_TABLE_ARRAY:
            {
                array = new vector<MetaValue *>{};
            } break;

            default:
            {
                assert(false);
            } break;
        }
    }


    ~MetaValue()
    {
        switch (type)
        {
            case META_TYPE_IMPLICIT_TABLE:
            case META_TYPE_DOTTED_TABLE:
            case META_TYPE_HEADER_TABLE:
            {
                delete table;
            } break;

            case META_TYPE_TABLE_ARRAY:
            {
                delete array;
            } break;

            default:
            {
                assert(type == META_TYPE_LITERAL);
            } break;
        }
    }
};


struct Parser final
{
    TokenList &tokens;
    u64 length;
    u64 position;

    Table &result;
    MetaValue meta;

    Table *current_value;
    MetaValue *current_meta;

    ErrorList &errors;

    explicit Parser(TokenList &token_list, Table &table, ErrorList &error_list)
        : tokens{token_list}
        , length{tokens.size()}
        , position{0}
        , result{table}
        , meta{META_TYPE_HEADER_TABLE}
        , current_value{&result}
        , current_meta{&meta}
        , errors{error_list}
    {
    }
};



//
// Predeclarations
//

void parse_keyval(Parser &parser, Table *table, MetaValue *meta);

Value *parse_value(Parser &parser);



//
// Implementation
//

const Token &
peek(const Parser &parser)
{
    assert(parser.position < parser.length);
    return parser.tokens[parser.position];
}


Token &
eat(Parser &parser, TokenType expected = TOKEN_ERROR)
{
    assert(parser.position < parser.length);
    assert((expected == TOKEN_ERROR) || (parser.tokens[parser.position].type == expected));
    return parser.tokens[parser.position++];
}


void
key_redefinition(Parser &parser, const Token &token)
{
    Error error = {token.line, token.column, "Key \"" + token.lexeme + "\" has already been defined."};
    parser.errors.push_back(move(error));
}


ArrayValue *
parse_array(Parser &parser)
{
    auto result = new ArrayValue{};
    eat(parser, TOKEN_LBRACKET);

    bool parsing = true;
    while (parsing)
    {
        const Token &token = peek(parser);

        switch (token.type)
        {
            case TOKEN_EOF:
            {
                // TODO: handle unterminated array
                assert(false);
            } break;

            case TOKEN_COMMA:
            {
                eat(parser);
                break;
            }

            case TOKEN_RBRACKET:
            {
                eat(parser);
                parsing = false;
            } break;

            default:
            {
                Value *value = parse_value(parser);
                result->value.push_back(value);
            } break;
        }
    }

    return result;
}


TableValue *
parse_inline_table(Parser &parser)
{
    auto result = new TableValue{};
    MetaValue meta{META_TYPE_HEADER_TABLE};

    eat(parser, TOKEN_LBRACE);

    bool parsing = true;
    while (parsing)
    {
        const Token &token = peek(parser);

        switch (token.type)
        {
            case TOKEN_EOF:
            {
                // TODO: handle unterminated table
                assert(false);
            } break;

            case TOKEN_COMMA:
            {
                eat(parser);
                break;
            }

            case TOKEN_RBRACE:
            {
                eat(parser);
                parsing = false;
            } break;

            default:
            {
                parse_keyval(parser, &result->value, &meta);
            } break;
        }
    }

    return result;
}


Value *
parse_value(Parser &parser)
{
    Value *result;

    const Token &token = peek(parser);
    switch (token.type)
    {
        case TOKEN_VALUE:
        {
            eat(parser);
            result = token.value;
        } break;

        case TOKEN_LBRACKET:
        {
            result = parse_array(parser);
        } break;

        case TOKEN_LBRACE:
        {
            result = parse_inline_table(parser);
        } break;

        default:
        {
            assert(false);
            result = new Value{};
        } break;
    }

    return result;
}


void
parse_keyval(Parser &parser, Table *table, MetaValue *table_meta)
{
    Token *key = &eat(parser, TOKEN_KEY);
    for ( ; peek(parser).type == TOKEN_KEY; key = &eat(parser))
    {
        Value *&value = (*table)[key->lexeme];
        MetaValue *&value_meta = (*table_meta->table)[key->lexeme];
        if (!value)
        {
            value = new TableValue();
            value_meta = new MetaValue(META_TYPE_DOTTED_TABLE);
        }
        else if (value_meta->type == META_TYPE_IMPLICIT_TABLE)
        {
            value_meta->type = META_TYPE_DOTTED_TABLE;
        }
        else if (value_meta->type != META_TYPE_DOTTED_TABLE)
        {
            key_redefinition(parser, *key);
            // Replacing the value and continuing on seems like maybe the
            // best way to mitigate cascading errors.
            delete value;
            delete value_meta;
            value = new TableValue();
            value_meta = new MetaValue(META_TYPE_DOTTED_TABLE);
        }

        assert(value->type == TYPE_TABLE);
        table = &static_cast<TableValue *>(value)->value;
        table_meta = value_meta;

    }

    Value *&value = (*table)[key->lexeme];
    MetaValue *&value_meta = (*table_meta->table)[key->lexeme];
    if (value)
    {
        key_redefinition(parser, *key);
        // Replacing the value and continuing on seems like maybe the best way
        // to mitigate cascading errors.
        delete value;
        delete value_meta;
    }

    value = parse_value(parser);
    value_meta = new MetaValue{};
}


Token *
parse_table_header(Parser &parser)
{
    Token *key = &eat(parser, TOKEN_KEY);
    for ( ; peek(parser).type == TOKEN_KEY; key = &eat(parser))
    {
        Value *&value = (*parser.current_value)[key->lexeme];
        MetaValue *&value_meta = (*parser.current_meta->table)[key->lexeme];
        if (!value)
        {
            value = new TableValue();
            value_meta = new MetaValue(META_TYPE_IMPLICIT_TABLE);

            parser.current_value = &static_cast<TableValue *>(value)->value;
            parser.current_meta = value_meta;
        }
        else
        {
            switch (value_meta->type)
            {
                case META_TYPE_TABLE_ARRAY:
                {
                    ArrayValue *array = static_cast<ArrayValue *>(value);
                    assert(array->value.size());
                    assert(value_meta->array->back()->type == META_TYPE_HEADER_TABLE);
                    parser.current_value = &static_cast<TableValue *>(array->value.back())->value;
                    parser.current_meta = value_meta->array->back();
                } break;

                case META_TYPE_IMPLICIT_TABLE:
                case META_TYPE_DOTTED_TABLE:
                case META_TYPE_HEADER_TABLE:
                {
                    assert(value->type == TYPE_TABLE);
                    parser.current_value = &static_cast<TableValue *>(value)->value;
                    parser.current_meta = value_meta;
                } break;

                default:
                {
                    key_redefinition(parser, *key);
                    // Replacing the value and continuing on seems like maybe the
                    // best way to mitigate cascading errors.
                    delete value;
                    delete value_meta;
                    value = new TableValue();
                    value_meta = new MetaValue(META_TYPE_IMPLICIT_TABLE);

                    parser.current_value = &static_cast<TableValue *>(value)->value;
                    parser.current_meta = value_meta;
                } break;
            }
        }
    }

    return key;
}


void
parse_table(Parser &parser)
{
    eat(parser, TOKEN_LBRACKET);

    parser.current_value = &parser.result;
    parser.current_meta = &parser.meta;
    Token *key = parse_table_header(parser);

    Value *&value = (*parser.current_value)[key->lexeme];
    MetaValue *&value_meta = (*parser.current_meta->table)[key->lexeme];
    if (!value)
    {
        value = new TableValue{};
        value_meta = new MetaValue{META_TYPE_HEADER_TABLE};
    }
    else if (value_meta->type == META_TYPE_IMPLICIT_TABLE)
    {
        value_meta->type = META_TYPE_HEADER_TABLE;
    }
    else
    {
        key_redefinition(parser, *key);
        delete value;
        delete value_meta;

        value = new TableValue();
        value_meta = new MetaValue(META_TYPE_HEADER_TABLE);
    }

    assert(value->type == TYPE_TABLE);
    parser.current_value = &static_cast<TableValue *>(value)->value;
    parser.current_meta = value_meta;

    eat(parser, TOKEN_RBRACKET);
}


void
parse_table_array(Parser &parser)
{
    eat(parser, TOKEN_DOUBLE_LBRACKET);

    parser.current_value = &parser.result;
    parser.current_meta = &parser.meta;
    Token *key = parse_table_header(parser);

    Value *&value = (*parser.current_value)[key->lexeme];
    MetaValue *&value_meta = (*parser.current_meta->table)[key->lexeme];
    if (!value)
    {
        value = new ArrayValue;
        value_meta = new MetaValue(META_TYPE_TABLE_ARRAY);
    }
    else if (value_meta->type != META_TYPE_TABLE_ARRAY)
    {
        key_redefinition(parser, *key);
        // Replacing the value and continuing on seems like maybe the best way
        // to mitigate cascading errors.
        delete value;
        delete value_meta;
        value = new ArrayValue;
        value_meta = new MetaValue(META_TYPE_TABLE_ARRAY);
    }

    auto table = new TableValue;
    auto table_meta = new MetaValue(META_TYPE_HEADER_TABLE);

    assert(value->type == TYPE_ARRAY);
    auto array = static_cast<ArrayValue *>(value);
    array->value.push_back(table);
    value_meta->array->push_back(table_meta);

    parser.current_value = &table->value;
    parser.current_meta = table_meta;

    eat(parser, TOKEN_DOUBLE_RBRACKET);
}


void
parse_expression(Parser &parser)
{
    switch (peek(parser).type)
    {
        case TOKEN_KEY:
        {
            parse_keyval(parser, parser.current_value, parser.current_meta);
        } break;

        case TOKEN_LBRACKET:
        {
            parse_table(parser);
        } break;

        case TOKEN_DOUBLE_LBRACKET:
        {
            parse_table_array(parser);
        } break;

        default:
        {
            assert(false); // Not reachable?
        } break;
    }
}


} // namespace


bool
parse_toml(const string &toml, Table &table, vector<Error> &errors)
{
    vector<Token> tokens;
    bool result = lex_toml(toml, tokens, errors);

    if (result)
    {
        Parser parser{tokens, table, errors};
        while (peek(parser).type != TOKEN_EOF)
        {
            parse_expression(parser);
        }

        result = errors.size() == 0;
    }

    return result;
}


} // namespace toml
