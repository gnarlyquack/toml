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


constexpr u32 FLAG_DEFINED = 1 << 0;
constexpr u32 FLAG_HEADER  = 1 << 1;
constexpr u32 FLAG_ARRAY   = 1 << 3;
constexpr u32 FLAG_TABLE   = 1 << 2;
constexpr u32 FLAG_LITERAL = 1 << 4;

constexpr u32 MASK_TYPE = FLAG_TABLE | FLAG_ARRAY | FLAG_LITERAL;


struct MetaValue final
{
    union
    {
        unordered_map<string, MetaValue *> *table;
        vector<MetaValue *> *array;
    };
    u32 flags;


    explicit MetaValue()
        : flags{FLAG_LITERAL | FLAG_DEFINED}
    {
    }


    explicit MetaValue(u32 f)
        : flags{f}
    {
        u32 type = flags & MASK_TYPE;
        switch (type)
        {
            case FLAG_ARRAY:
            {
                array = new vector<MetaValue *>{};
            } break;

            case FLAG_TABLE:
            {
                table = new unordered_map<string, MetaValue *>{};
            } break;

            default:
            {
                assert(type == FLAG_LITERAL);
            } break;
        }
    }


    ~MetaValue()
    {
        u32 type = flags & MASK_TYPE;
        switch (type)
        {
            case FLAG_ARRAY:
            {
                delete array;
            } break;

            case FLAG_TABLE:
            {
                delete table;
            } break;

            default:
            {
                assert(type == FLAG_LITERAL);
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
        , meta{FLAG_TABLE}
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
    MetaValue meta{FLAG_TABLE};

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


Token *
parse_key(Parser &parser, Table *&table, MetaValue *&table_meta, u32 flags)
{
    Token *key = &eat(parser, TOKEN_KEY);

    while (peek(parser).type == TOKEN_KEY)
    {
        Value *value = (*table)[key->lexeme];
        MetaValue *value_meta = (*table_meta->table)[key->lexeme];
        if (!value)
        {
            value = new TableValue();
            value_meta = new MetaValue{flags};
            (*table)[key->lexeme] = value;
            (*table_meta->table)[key->lexeme] = value_meta;
        }
        else if (value->type == TYPE_ARRAY)
        {
            if ((flags & FLAG_HEADER) && (value_meta->flags & FLAG_HEADER))
            {
                ArrayValue *array = static_cast<ArrayValue *>(value);
                assert(array->value.size());
                value = array->value.back();
                value_meta = value_meta->array->back();
                assert(value->type == TYPE_TABLE);
            }
            else
            {
                key_redefinition(parser, *key);
                // Replacing the value and continuing on seems like maybe the
                // best way to mitigate cascading errors.
                delete value;
                delete value_meta;
                value = new TableValue();
                value_meta = new MetaValue{flags};
                (*table)[key->lexeme] = value;
                (*table_meta->table)[key->lexeme] = value_meta;
            }
        }
        else if (value->type != TYPE_TABLE)
        {
            key_redefinition(parser, *key);
            // Replacing the value and continuing on seems like maybe the
            // best way to mitigate cascading errors.
            delete value;
            delete value_meta;
            value = new TableValue();
            value_meta = new MetaValue{flags};
            (*table)[key->lexeme] = value;
            (*table_meta->table)[key->lexeme] = value_meta;
        }
        else if (value_meta->flags & FLAG_LITERAL)
        {
            // TODO: Handle trying to extend an inline table
            key_redefinition(parser, *key);
            // Replacing the value and continuing on seems like maybe the
            // best way to mitigate cascading errors.
            value_meta->flags = FLAG_TABLE;
            value_meta->table = new unordered_map<string, MetaValue *>{};
            (*table)[key->lexeme] = value;
            (*table_meta->table)[key->lexeme] = value_meta;
        }

        assert(value->type == TYPE_TABLE);
        table = &static_cast<TableValue *>(value)->value;
        table_meta = value_meta;

        key = &eat(parser);
    }

    return key;
}


void
parse_keyval(Parser &parser, Table *table, MetaValue *table_meta)
{
    Token *key = parse_key(parser, table, table_meta, FLAG_TABLE);

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


void
parse_table(Parser &parser)
{
    eat(parser, TOKEN_LBRACKET);
    parser.current_value = &parser.result;
    parser.current_meta = &parser.meta;

    Token *key = parse_key(parser, parser.current_value, parser.current_meta, FLAG_TABLE | FLAG_HEADER);

    Value *&value = (*parser.current_value)[key->lexeme];
    MetaValue *&value_meta = (*parser.current_meta->table)[key->lexeme];
    if (!value)
    {
        value = new TableValue{};
        value_meta = new MetaValue{FLAG_TABLE | FLAG_HEADER | FLAG_DEFINED};
    }
    else if (value->type != TYPE_TABLE)
    {
        key_redefinition(parser, *key);
        // Replacing the value and continuing on seems like maybe the best way
        // to mitigate cascading errors.
        delete value;
        delete value_meta;
        value = new TableValue();
        value_meta = new MetaValue{FLAG_TABLE | FLAG_HEADER | FLAG_DEFINED};
    }
    else if (value_meta->flags & FLAG_DEFINED)
    {
        key_redefinition(parser, *key);
    }
    else if ((value_meta->flags & FLAG_HEADER) == 0)
    {
        key_redefinition(parser, *key);
        //value_meta->defined = true;
    }
    else
    {
        value_meta->flags |= FLAG_DEFINED;
    }

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

    Token *key = parse_key(parser, parser.current_value, parser.current_meta, FLAG_TABLE | FLAG_HEADER);

    Value *&value = (*parser.current_value)[key->lexeme];
    MetaValue *&value_meta = (*parser.current_meta->table)[key->lexeme];
    if (!value)
    {
        value = new ArrayValue{};
        value_meta = new MetaValue{FLAG_ARRAY | FLAG_HEADER};
    }
    else if (value->type != TYPE_ARRAY)
    {
        key_redefinition(parser, *key);
        // Replacing the value and continuing on seems like maybe the best way
        // to mitigate cascading errors.
        delete value;
        delete value_meta;
        value = new ArrayValue{};
        value_meta = new MetaValue{FLAG_ARRAY | FLAG_HEADER};
    }
    else if ((value_meta->flags & FLAG_ARRAY) == 0)
    {
        // TODO Handle trying to extend array literal
        key_redefinition(parser, *key);
        // Replacing the value and continuing on seems like maybe the best way
        // to mitigate cascading errors.
        value_meta->flags = FLAG_ARRAY | FLAG_HEADER;
        value_meta->array = new vector<MetaValue *>{};
    }

    auto table = new TableValue{};
    auto table_meta = new MetaValue{FLAG_TABLE};

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
