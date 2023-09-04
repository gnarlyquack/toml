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


struct Value
{
    enum class Type
    {
        INVALID,

        ARRAY,
        BOOL,
        FLOAT,
        INTEGER,
        LOCAL_DATE,
        LOCAL_DATETIME,
        LOCAL_TIME,
        OFFSET_DATETIME,
        STRING,
        TABLE,
    };

    const Type type;


    explicit Value() : type(Type::INVALID)
    {
    }

protected:
    explicit Value(Type t) : type(t)
    {
    }
};


struct ArrayValue final : public Value
{
    Array value;


    explicit ArrayValue() : Value{Type::ARRAY}, value{}
    {
    }


    explicit ArrayValue(const Array &array) : Value{Type::ARRAY}, value{array}
    {
    }

    explicit ArrayValue(Array &&array) : Value{Type::ARRAY}, value{std::move(array)}
    {
    }
};


struct BooleanValue final : public Value
{
    bool value;


    explicit BooleanValue() : Value(Type::BOOL), value(false)
    {
    }


    explicit BooleanValue(bool boolean) : Value(Type::BOOL), value(boolean)
    {
    }
};


struct FloatValue final : public Value
{
    double value;


    explicit FloatValue() : Value(Type::FLOAT), value(0)
    {
    }


    explicit FloatValue(double number) : Value(Type::FLOAT) , value(number)
    {
    }
};


struct IntegerValue final : public Value
{
    int64_t value;


    explicit IntegerValue() : Value(Type::INTEGER), value(0)
    {
    }


    explicit IntegerValue(int64_t integer) : Value(Type::INTEGER) , value(integer)
    {
    }
};


struct StringValue final : public Value
{
    std::string value;


    explicit StringValue() : Value(Type::STRING), value()
    {
    }


    explicit StringValue(const std::string &str) : Value(Type::STRING) , value(str)
    {
    }


    explicit StringValue(std::string &&str) : Value(Type::STRING) , value(std::move(str))
    {
    }
};


struct LocalDateValue final : public Value
{
    LocalDate value;


    explicit LocalDateValue() : Value(Type::LOCAL_DATE), value()
    {
    }


    explicit LocalDateValue(const LocalDate &date) : Value(Type::LOCAL_DATE), value(date)
    {
    }

    explicit LocalDateValue(LocalDate &&date) : Value(Type::LOCAL_DATE), value(std::move(date))
    {
    }
};


struct LocalDateTimeValue final : public Value
{
    LocalDateTime value;


    explicit LocalDateTimeValue() : Value(Type::LOCAL_DATETIME), value()
    {
    }


    explicit LocalDateTimeValue(const LocalDateTime &datetime) : Value(Type::LOCAL_DATETIME), value(datetime)
    {
    }

    explicit LocalDateTimeValue(LocalDateTime &&datetime) : Value(Type::LOCAL_DATETIME), value(std::move(datetime))
    {
    }
};


struct LocalTimeValue final : public Value
{
    LocalTime value;


    explicit LocalTimeValue() : Value(Type::LOCAL_TIME), value()
    {
    }


    explicit LocalTimeValue(const LocalTime &time) : Value(Type::LOCAL_TIME), value(time)
    {
    }

    explicit LocalTimeValue(LocalTime &&time) : Value(Type::LOCAL_TIME), value(std::move(time))
    {
    }
};


struct OffsetDateTimeValue final : public Value
{
    OffsetDateTime value;


    explicit OffsetDateTimeValue() : Value(Type::OFFSET_DATETIME), value()
    {
    }


    explicit OffsetDateTimeValue(const OffsetDateTime &datetime) : Value(Type::OFFSET_DATETIME), value(datetime)
    {
    }

    explicit OffsetDateTimeValue(OffsetDateTime &&datetime) : Value(Type::OFFSET_DATETIME), value(std::move(datetime))
    {
    }
};


struct TableValue final : public Value
{
    Table value;


    explicit TableValue() : Value(Type::TABLE), value()
    {
    }


    explicit TableValue(const Table &table) : Value(Type::TABLE) , value(table)
    {
    }


    explicit TableValue(Table &&table) : Value(Type::TABLE) , value(std::move(table))
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
    Value::Type type;
    uint32_t line;
    uint32_t column;
    union
    {
        Definitions *definitions;
        std::vector<Record *> *records;
        Value *value;
    };

    Record(Value::Type t, uint32_t l, uint32_t c)
        : type{t}
        , line{l}
        , column{c}
    {
        switch (type)
        {
            case Value::Type::ARRAY:
            {
                records = new std::vector<Record *>;
            } break;

            case Value::Type::TABLE:
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
        assert((type != Value::Type::ARRAY) || (type != Value::Type::TABLE));
    }
};


struct Definition
{
    Key key;
    Record *record;
};




} // namespace toml


#endif // TOML_TYPES_HPP
