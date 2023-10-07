//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_PARSER_HPP
#define TOML_PARSER_HPP

#include "types.hpp"
#include "lexer.hpp"

#include <string>
#include <unordered_map>
#include <vector>



namespace toml
{


struct ValueMeta;

using ArrayMeta = std::vector<ValueMeta>;
using TableMeta = std::unordered_map<std::string, ValueMeta>;


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

    std::vector<Error> &errors;

    explicit Parser(Lexer &l, DefinitionTable &definitions, std::vector<Error> &error_list)
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


void
advance(Parser &parser, u32 context);


bool
parse_toml(const std::string &toml, Table &result, std::vector<Error> &errors);


bool
parse_with_metadata(const std::string &toml, DefinitionTable &definitions, std::vector<Error> &errors);


} // namespace toml


#endif // TOML_PARSER_HPP
