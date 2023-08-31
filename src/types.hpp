//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TYPES_HPP
#define TOML_TYPES_HPP

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility> // move
#include <vector>

#include <date/tz.h>


namespace toml
{


struct Value;

using Array = std::vector<Value *>;
using LocalDate = date::local_time<date::days>;
using LocalDateTime = date::local_time<std::chrono::microseconds>;
using LocalTime = date::local_time<std::chrono::microseconds>;
using OffsetDateTime = date::sys_time<std::chrono::microseconds>;
using Table = std::unordered_map<std::string, Value *>;


enum ValueType
{
    TYPE_INVALID,

    TYPE_ARRAY,
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_INTEGER,
    TYPE_LOCAL_DATE,
    TYPE_LOCAL_DATETIME,
    TYPE_LOCAL_TIME,
    TYPE_OFFSET_DATETIME,
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


struct ArrayValue final : public Value
{
    Array value;


    explicit ArrayValue() : Value{TYPE_ARRAY}, value{}
    {
    }


    explicit ArrayValue(const Array &array) : Value{TYPE_ARRAY}, value{array}
    {
    }

    explicit ArrayValue(Array &&array) : Value{TYPE_ARRAY}, value{std::move(array)}
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


struct LocalDateValue final : public Value
{
    LocalDate value;


    explicit LocalDateValue() : Value(TYPE_LOCAL_DATE), value()
    {
    }


    explicit LocalDateValue(const LocalDate &date) : Value(TYPE_LOCAL_DATE), value(date)
    {
    }

    explicit LocalDateValue(LocalDate &&date) : Value(TYPE_LOCAL_DATE), value(std::move(date))
    {
    }
};


struct LocalDateTimeValue final : public Value
{
    LocalDateTime value;


    explicit LocalDateTimeValue() : Value(TYPE_LOCAL_DATETIME), value()
    {
    }


    explicit LocalDateTimeValue(const LocalDateTime &datetime) : Value(TYPE_LOCAL_DATETIME), value(datetime)
    {
    }

    explicit LocalDateTimeValue(LocalDateTime &&datetime) : Value(TYPE_LOCAL_DATETIME), value(std::move(datetime))
    {
    }
};


struct LocalTimeValue final : public Value
{
    LocalTime value;


    explicit LocalTimeValue() : Value(TYPE_LOCAL_TIME), value()
    {
    }


    explicit LocalTimeValue(const LocalTime &time) : Value(TYPE_LOCAL_TIME), value(time)
    {
    }

    explicit LocalTimeValue(LocalTime &&time) : Value(TYPE_LOCAL_TIME), value(std::move(time))
    {
    }
};


struct OffsetDateTimeValue final : public Value
{
    OffsetDateTime value;


    explicit OffsetDateTimeValue() : Value(TYPE_OFFSET_DATETIME), value()
    {
    }


    explicit OffsetDateTimeValue(const OffsetDateTime &datetime) : Value(TYPE_OFFSET_DATETIME), value(datetime)
    {
    }

    explicit OffsetDateTimeValue(OffsetDateTime &&datetime) : Value(TYPE_OFFSET_DATETIME), value(std::move(datetime))
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


struct Definition;


using Definitions = std::unordered_map<std::string, Definition *>;


struct Key
{
    std::string value;
    uint32_t line;
    uint32_t column;
};


struct Record
{
    ValueType type;
    uint32_t line;
    uint32_t column;
    union
    {
        Definitions *definitions;
        std::vector<Record *> *records;
        Value *value;
    };

    Record(ValueType t, uint32_t l, uint32_t c)
        : type{t}
        , line{l}
        , column{c}
    {
        switch (type)
        {
            case TYPE_ARRAY:
            {
                records = new std::vector<Record *>;
            } break;

            case TYPE_TABLE:
            {
                definitions = new Definitions;
            } break;

            default:
            {
                assert(false);
            } break;
        }
    }

    Record(Value *v, uint32_t l, uint32_t c)
        : type{v->type}
        , line{l}
        , column{c}
        , value{v}
    {
        assert((type != TYPE_ARRAY) || (type != TYPE_TABLE));
    }
};


struct Definition
{
    Key key;
    Record *record;
};




} // namespace toml


#endif // TOML_TYPES_HPP
