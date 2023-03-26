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


Value *
lex_value(Parser &parser)
{
    Value *result;

    switch (peek(parser).type)
    {
        case TOKEN_DECIMAL:
        {
            Token &value = eat(parser);
            result = new IntegerValue(string_to_u64(value.lexeme));
        } break;

        case TOKEN_FALSE:
        {
            eat(parser);
            result = new BooleanValue(false);
        } break;

        case TOKEN_STRING:
        {
            Token &value = eat(parser);
            result = new StringValue(move(value.lexeme));
        } break;

        case TOKEN_TRUE:
        {
            eat(parser);
            result = new BooleanValue(true);
        } break;

        default:
        {
            assert(false); // unreachable
        } break;
    }

    return result;
}


void
parse_keyval(Parser &parser)
{
    Table *table = &parser.result;
    Token *key = &eat(parser, TOKEN_KEY);

    while (peek(parser).type == TOKEN_KEY)
    {
        Value* &value = (*table)[key->lexeme];
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
    value = lex_value(parser);
}


void
parse_expression(Parser &parser)
{
    switch (peek(parser).type)
    {
        case TOKEN_KEY:
        {
            parse_keyval(parser);
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
