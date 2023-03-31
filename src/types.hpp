//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TYPES_HPP
#define TOML_TYPES_HPP

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility> // move


namespace toml
{


struct Value;

using Table = std::unordered_map<std::string, Value *>;


enum ValueType
{
    TYPE_INVALID,

    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_TABLE,
};


struct Value
{
    const ValueType type;


    explicit Value() : type(TYPE_INVALID)
    {
    }

protected:
    explicit Value(ValueType t) : type(t)
    {
    }
};


struct BooleanValue final : public Value
{
    bool value;


    explicit BooleanValue() : Value(TYPE_BOOL), value(false)
    {
    }


    explicit BooleanValue(bool boolean) : Value(TYPE_BOOL), value(boolean)
    {
    }
};


struct FloatValue final : public Value
{
    double value;


    explicit FloatValue() : Value(TYPE_FLOAT), value(0)
    {
    }


    explicit FloatValue(double number) : Value(TYPE_FLOAT) , value(number)
    {
    }
};


struct IntegerValue final : public Value
{
    int64_t value;


    explicit IntegerValue() : Value(TYPE_INTEGER), value(0)
    {
    }


    explicit IntegerValue(int64_t integer) : Value(TYPE_INTEGER) , value(integer)
    {
    }
};


struct StringValue final : public Value
{
    std::string value;


    explicit StringValue() : Value(TYPE_STRING), value()
    {
    }


    explicit StringValue(const std::string &str) : Value(TYPE_STRING) , value(str)
    {
    }


    explicit StringValue(std::string &&str) : Value(TYPE_STRING) , value(std::move(str))
    {
    }
};


struct TableValue final : public Value
{
    Table value;


    explicit TableValue() : Value(TYPE_TABLE), value()
    {
    }


    explicit TableValue(const Table &table) : Value(TYPE_TABLE) , value(table)
    {
    }


    explicit TableValue(Table &&table) : Value(TYPE_TABLE) , value(std::move(table))
    {
    }
};


} // namespace toml


#endif // TOML_TYPES_HPP
