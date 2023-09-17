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


struct ValueMeta;

using ArrayMeta = vector<ValueMeta>;
using TableMeta = unordered_map<string, ValueMeta>;


struct ValueMeta final
{
    enum class Type {
        INVALID,
        LITERAL,
        IMPLICIT_TABLE,
        DOTTED_TABLE,
        HEADER_TABLE,
        TABLE_ARRAY,
    };


    explicit ValueMeta()
        : _type(Type::INVALID)
    {
    }


    explicit ValueMeta(Type type)
        : _type(type)
    {
        switch (_type)
        {
            case Type::IMPLICIT_TABLE:
            case Type::DOTTED_TABLE:
            case Type::HEADER_TABLE:
            {
                _table = new TableMeta;
            } break;

            case Type::TABLE_ARRAY:
            {
                _array = new ArrayMeta;
            } break;

            default:
            {
                assert(_type == Type::LITERAL);
            } break;
        }
    }


    ValueMeta(const ValueMeta &other)
        : _type(other._type)
    {
        switch (_type)
        {
            case Type::IMPLICIT_TABLE:
            case Type::DOTTED_TABLE:
            case Type::HEADER_TABLE:
            {
                _table = new TableMeta(*other._table);
            } break;

            case Type::TABLE_ARRAY:
            {
                _array = new ArrayMeta(*other._array);
            } break;

            default:
            {
                // do nothing
            } break;
        }
    }


    ValueMeta(ValueMeta &&other)
    {
        take(other);
    }


    ~ValueMeta()
    {
        destroy();
    }


    ValueMeta &
    operator=(const ValueMeta &other)
    {
        if (this != &other)
        {
            ValueMeta temp(other);
            destroy();
            take(temp);
        }
        return *this;
    }


    ValueMeta &
    operator=(ValueMeta &&other)
    {
        if (this != &other)
        {
            destroy();
            take(other);
        }
        return *this;
    }


    void
    make_table_dotted()
    {
        assert(_type == Type::IMPLICIT_TABLE);
        _type = Type::DOTTED_TABLE;
    }


    void
    make_table_header()
    {
        assert(_type == Type::IMPLICIT_TABLE);
        _type = Type::HEADER_TABLE;
    }


    Type
    type() const
    {
        return _type;
    }


    ArrayMeta &
    array()
    {
        assert(_type == Type::TABLE_ARRAY);
        return *_array;
    }


    const ArrayMeta &
    array() const
    {
        assert(_type == Type::TABLE_ARRAY);
        return *_array;
    }


    TableMeta &
    table()
    {
        assert((_type == Type::IMPLICIT_TABLE) || (_type == Type::DOTTED_TABLE) || (_type == Type::HEADER_TABLE));
        return *_table;
    }


    const TableMeta &
    table() const
    {
        assert((_type == Type::IMPLICIT_TABLE) || (_type == Type::DOTTED_TABLE) || (_type == Type::HEADER_TABLE));
        return *_table;
    }


private:

    Type _type;

    union
    {
        ArrayMeta *_array;
        TableMeta *_table;
    };


    void
    take(ValueMeta &other)
    {
        switch (other._type)
        {
            case Type::IMPLICIT_TABLE:
            case Type::DOTTED_TABLE:
            case Type::HEADER_TABLE:
            {
                _table = other._table;
            } break;

            case Type::TABLE_ARRAY:
            {
                _array = other._array;
            } break;

            default:
            {
                // do nothing
            } break;
        }

        _type = other._type;
        other._type = Type::INVALID;
    }


    void
    destroy()
    {
        switch (_type)
        {
            case Type::IMPLICIT_TABLE:
            case Type::DOTTED_TABLE:
            case Type::HEADER_TABLE:
            {
                delete _table;
            } break;

            case Type::TABLE_ARRAY:
            {
                delete _array;
            } break;

            default:
            {
                // do nothing
            } break;
        }
    }
};


struct Parser final
{
    Lexer &lexer;
    Token token;

    DefinitionTable &table;
    TableMeta meta;

    DefinitionTable *current_table;
    TableMeta *current_meta;

    ErrorList &errors;

    explicit Parser(Lexer &l, DefinitionTable &definitions, ErrorList &error_list)
        : lexer(l)
        , token()
        , table(definitions)
        , meta()
        , current_table(&table)
        , current_meta(&meta)
        , errors(error_list)
    {
    }
};



//
// Predeclarations
//

void parse_keyval(Parser &parser, DefinitionTable *table, TableMeta *meta, u32 context);

Definition parse_value(Parser &parser, u32 context);



//
// Implementation
//

#if 0
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
#endif

void
add_error(Parser &parser, string message)
{
    Token token = parser.token;
    Error error = {token.location, move(message)};
    parser.errors.push_back(move(error));
}


void
advance(Parser &parser, u32 context, TokenType expected = TOKEN_ERROR)
{
    assert((expected == TOKEN_ERROR) || (expected == parser.token.type));
    parser.token = next_token(parser.lexer, context);
}


void
resynchronize(Parser &parser, string message, u32 context)
{
    add_error(parser, move(message));
    resynchronize(parser.lexer, context);
    advance(parser, context);
}


Token
eat(Parser &parser, u32 context, TokenType expected = TOKEN_ERROR)
{
    assert((expected == TOKEN_ERROR) || (parser.token.type == expected));

    Token result = parser.token;
    advance(parser, context);
    return result;
}


bool
match(Parser &parser, TokenType expected)
{
    bool result = parser.token.type == expected;
    return result;
}


TokenType
current_token_type(Parser &parser)
{
    TokenType result = parser.token.type;
    return result;
}


void
key_redefinition(Parser &parser, const Key &key, const Key &prev)
{
    Error error = {key.location, "Key '" + key.value + "' has already been defined on line " + to_string(prev.location.line) + ", character " + to_string(prev.location.column) + "."};
    parser.errors.push_back(move(error));
}


void
missing_value(Parser &parser)
{
    Token token = parser.token;
    Error error = {token.location, "Missing value."};
    parser.errors.push_back(move(error));
}


Token
make_error(Parser &parser, TokenType type, string message)
{
    add_error(parser, move(message));
    Token result = { type, Value(), "", SourceLocation() };
    return result;
}


Definition
array_definition(const Token &token)
{
    Definition result(Value::Type::ARRAY, token.location);
    return result;
}


Definition
table_definition(const Token &token)
{
    Definition result(Value::Type::TABLE, token.location);
    return result;
}


Definition
value_definition(const Token &token)
{
    assert(token.type == TOKEN_VALUE);
    Definition result(token.value, token.location);
    return result;
}


Definition
parse_array(Parser &parser, u32 context)
{
    Definition result = array_definition(eat(parser, context | LEX_ARRAY | LEX_VALUE, TOKEN_LBRACKET));
    TokenType prev = TOKEN_LBRACKET;
    bool parsing = true;
    while (parsing)
    {
        TokenType type = current_token_type(parser);
        switch (type)
        {
            case TOKEN_COMMA:
            {
                if (prev != TOKEN_VALUE)
                {
                    add_error(parser, "Missing array value.");
                }
                eat(parser, context | LEX_ARRAY | LEX_VALUE);
                prev = TOKEN_COMMA;
            } break;

            case TOKEN_RBRACKET:
            {
                eat(parser, context);
                parsing = false;
            } break;

            case TOKEN_COMMENT:
            case TOKEN_NEWLINE:
            {
                advance(parser, context | LEX_ARRAY | LEX_VALUE);
            } break;

            case TOKEN_EOF:
            {
                // TODO: handle unterminated array
                assert(false);
                parsing = false;
            } break;

            default:
            {
                if (prev == TOKEN_VALUE)
                {
                    add_error(parser, "Missing ',' between array values.");
                }
                Definition value = parse_value(parser, context | LEX_ARRAY);
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
    auto result = table_definition(eat(parser, context | LEX_TABLE | LEX_KEY, TOKEN_LBRACE));
    TableMeta meta;
    TokenType prev = TOKEN_LBRACE;
    bool parsing = true;
    while (parsing)
    {
        TokenType type = current_token_type(parser);
        switch (type)
        {
            case TOKEN_COMMA:
            {
                if (prev != TOKEN_VALUE)
                {
                    add_error(parser, "Missing inline table value.");
                }
                eat(parser, context | LEX_TABLE | LEX_KEY);
                prev = TOKEN_COMMA;
            } break;

            case TOKEN_RBRACE:
            {
                if (prev == TOKEN_COMMA)
                {
                    add_error(parser, "Trailing ',' is not allowed in an inline table.");
                }
                eat(parser, context);
                parsing = false;
            } break;

            case TOKEN_EOF:
            {
                // TODO: handle unterminated table
                assert(false);
            } break;

            default:
            {
                if (prev == TOKEN_VALUE)
                {
                    add_error(parser, "Missing ',' between inline table values.");
                }
                parse_keyval(parser, &result.as_table(), &meta, LEX_TABLE);
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

    switch (current_token_type(parser))
    {
        case TOKEN_VALUE:
        {
            result = value_definition(eat(parser, context));
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

        default:
        {
            resynchronize(parser, "Invalid value.", LEX_EOL);
        } break;
    }

    return result;
}


void
parse_keyval(Parser &parser, DefinitionTable *table, TableMeta *meta, u32 context)
{
    Token token;
    if (current_token_type(parser) == TOKEN_KEY)
    {
        token = eat(parser, context | LEX_KEY | LEX_VALUE, TOKEN_KEY);
    }
    else
    {
        token = make_error(parser, TOKEN_KEY, "Missing key.");
    }

    while (current_token_type(parser) == TOKEN_PERIOD)
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
            key_redefinition(parser, key, table->find(key)->first);
            // Replacing the value and continuing on seems like maybe the
            // best way to mitigate cascading errors.
            value = Definition(Value::Type::TABLE, key.location);
            value_meta = ValueMeta(ValueMeta::Type::DOTTED_TABLE);
        }

        assert(value.type() == Value::Type::TABLE);
        table = &value.as_table();

        assert(value_meta.type() == ValueMeta::Type::DOTTED_TABLE);
        meta = &value_meta.table();

        eat(parser, context | LEX_KEY, TOKEN_PERIOD);
        token = eat(parser, context | LEX_KEY | LEX_VALUE);
    }

    Key key = {token.lexeme, token.location};
    Definition &value = (*table)[key];
    ValueMeta &value_meta = (*meta)[key.value];
    if (value.type() != Value::Type::INVALID)
    {
        key_redefinition(parser, key, table->find(key)->first);
    }

    assert(parser.token.type == TOKEN_EQUAL);
    advance(parser, context | LEX_VALUE);

    value = parse_value(parser, context);
    value_meta = ValueMeta(ValueMeta::Type::LITERAL);
}


Key
parse_table_header(Parser &parser)
{
    Token token = eat(parser, LEX_HEADER);

    while (parser.token.type == TOKEN_PERIOD)
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
                    key_redefinition(parser, key, parser.current_table->find(key)->first);
                    // Replacing the value and continuing on seems like maybe the
                    // best way to mitigate cascading errors.
                    value = Definition(Value::Type::TABLE, key.location);
                    value_meta = ValueMeta(ValueMeta::Type::IMPLICIT_TABLE);

                    parser.current_table = &value.as_table();
                    parser.current_meta = &value_meta.table();
                } break;
            }
        }

        eat(parser, LEX_HEADER, TOKEN_PERIOD);
        token = eat(parser, LEX_HEADER);
    }

    Key key = {token.lexeme, token.location};
    return key;
}


void
parse_table(Parser &parser)
{
    eat(parser, LEX_HEADER, TOKEN_LBRACKET);

    parser.current_table = &parser.table;
    parser.current_meta = &parser.meta;
    Key key = parse_table_header(parser);

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

    eat(parser, LEX_EOL, TOKEN_RBRACKET);
}


void
parse_table_array(Parser &parser)
{
    eat(parser, LEX_HEADER, TOKEN_DOUBLE_LBRACKET);

    parser.current_table = &parser.table;
    parser.current_meta = &parser.meta;
    Key key = parse_table_header(parser);

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

    eat(parser, LEX_EOL, TOKEN_DOUBLE_RBRACKET);
}


void
parse_expression(Parser &parser)
{
    switch (current_token_type(parser))
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

        // invalid cases

        case TOKEN_EQUAL:
        {
            parse_keyval(parser, parser.current_table, parser.current_meta, LEX_EOL);
        } break;

        default:
        {
            // TODO: Handle error here?
        } break;
    }

    if (current_token_type(parser) == TOKEN_COMMENT)
    {
        eat(parser, LEX_EOL);
    }

    switch (current_token_type(parser))
    {
        case TOKEN_COMMENT:
        {
            eat(parser, LEX_EOL);
        } // fallthrough

        case TOKEN_NEWLINE:
        case TOKEN_EOF:
        {
            // do nothing
        } break;

        default:
        {
            resynchronize(parser, "Expected the end of the line.", LEX_EOL);
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

#if 0

    vector<Token> tokens;
    bool result = lex_toml(toml, tokens, errors);

    if (result)
    {
        Parser parser{tokens, definitions, errors};
        while (peek(parser).type != TOKEN_EOF)
        {
            parse_expression(parser);
        }

        result = errors.size() == 0;
    }

#else

    Lexer lexer(toml, errors);
    Parser parser(lexer, definitions, errors);

    for (advance(parser, LEX_HEADER | LEX_KEY); !match(parser, TOKEN_EOF); advance(parser, LEX_HEADER | LEX_KEY))
    {
        parse_expression(parser);
    }

#endif

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
