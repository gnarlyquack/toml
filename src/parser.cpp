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


struct Parser
{
    TokenList &tokens;
    u64 length;
    u64 position;

    Table &result;
    ErrorList &errors;
};



//
// Predeclarations
//

void parse_keyval(Parser &parser, Table *table);

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
                parse_keyval(parser, &result->value);
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
parse_keyval(Parser &parser, Table *table)
{
    Token *key = &eat(parser, TOKEN_KEY);

    while (peek(parser).type == TOKEN_KEY)
    {
        Value *&value = (*table)[key->lexeme];
        if (value)
        {
            if (value->type != TYPE_TABLE)
            {
                key_redefinition(parser, *key);
                // Simply replacing the value and continuing on seems like
                // maybe be the best way to mitigate cascading errors.
                delete value;
                value = new TableValue();
            }
        }
        else
        {
            value = new TableValue();
        }

        assert(value->type == TYPE_TABLE);
        table = &static_cast<TableValue *>(value)->value;

        key = &eat(parser);
    }

    Value* &value = (*table)[key->lexeme];
    if (value)
    {
        key_redefinition(parser, *key);
        delete value;
    }

    value = parse_value(parser);
}


void
parse_expression(Parser &parser)
{
    switch (peek(parser).type)
    {
        case TOKEN_KEY:
        {
            parse_keyval(parser, &parser.result);
        } break;

        case TOKEN_LBRACKET:
        {
            fputs("Implement parse_table\n", stderr);
            assert(false);
        } break;

        case TOKEN_DOUBLE_LBRACKET:
        {
            fputs("Implement parse_table_array\n", stderr);
            assert(false);
        } break;

        default:
        {
            assert(false); // Not reachable?
        } break;
    }
}


} // namespace


bool
parse_toml(const std::string &toml, Table &table, std::vector<Error> &errors)
{
    vector<Token> tokens;
    bool result = lex_toml(toml, tokens, errors);

    if (result)
    {
        Parser parser = { tokens, tokens.size(), 0, table, errors };
        while (peek(parser).type != TOKEN_EOF)
        {
            parse_expression(parser);
        }

        result = errors.size() == 0;
    }

    return result;
}


} // namespace toml
