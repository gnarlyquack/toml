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
        BOOLEAN,
        FLOAT,
        INTEGER,
        LOCAL_DATE,
        LOCAL_DATETIME,
        LOCAL_TIME,
        OFFSET_DATETIME,
        STRING,
        TABLE,
    };


    explicit Value() : _type(Type::INVALID)
    {
    }


    virtual ~Value() = default;


    Type
    type() const
    {
        return _type;
    }


    Array &
    as_array()
    {
        assert(_type == Type::ARRAY);
        return *_array;
    }


    const Array &
    as_array() const
    {
        assert(_type == Type::ARRAY);
        return *_array;
    }


    bool
    as_boolean() const
    {
        assert(_type == Type::BOOLEAN);
        return _boolean;
    }


    double
    as_float() const
    {
        assert(_type == Type::FLOAT);
        return _float;
    }


    int64_t
    as_integer() const
    {
        assert(_type == Type::INTEGER);
        return _integer;
    }


    std::string &
    as_string()
    {
        assert(_type == Type::STRING);
        return *_string;
    }


    const std::string &
    as_string() const
    {
        assert(_type == Type::STRING);
        return *_string;
    }


    LocalDate &
    as_local_date()
    {
        assert(_type == Type::LOCAL_DATE);
        return _local_date;
    }


    const LocalDate &
    as_local_date() const
    {
        assert(_type == Type::LOCAL_DATE);
        return _local_date;
    }


    LocalDateTime &
    as_local_datetime()
    {
        assert(_type == Type::LOCAL_DATETIME);
        return _local_datetime;
    }


    const LocalDateTime &
    as_local_datetime() const
    {
        assert(_type == Type::LOCAL_DATETIME);
        return _local_datetime;
    }


    LocalTime &
    as_local_time()
    {
        assert(_type == Type::LOCAL_TIME);
        return _local_time;
    }


    const LocalTime &
    as_local_time() const
    {
        assert(_type == Type::LOCAL_TIME);
        return _local_time;
    }


    OffsetDateTime &
    as_offset_datetime()
    {
        assert(_type == Type::OFFSET_DATETIME);
        return _offset_datetime;
    }


    const OffsetDateTime &
    as_offset_datetime() const
    {
        assert(_type == Type::OFFSET_DATETIME);
        return _offset_datetime;
    }

    Table &
    as_table()
    {
        assert(_type == Type::TABLE);
        return *_table;
    }


    const Table &
    as_table() const
    {
        assert(_type == Type::TABLE);
        return *_table;
    }


protected:
    explicit Value(Type type) : _type(type)
    {
    }


    Type _type;

    union
    {
        bool _boolean;
        int64_t _integer;
        double _float;
        LocalDate _local_date;
        LocalDateTime _local_datetime;
        LocalTime _local_time;
        OffsetDateTime _offset_datetime;
        std::string *_string;
        Array *_array;
        Table *_table;
    };
};


struct ArrayValue final : public Value
{
    explicit ArrayValue()
        : Value{Type::ARRAY}
    {
        _array = new Array;
    }


    explicit ArrayValue(const Array &array)
        : Value{Type::ARRAY}
    {
        _array = new Array{array};
    }


    explicit ArrayValue(Array &&array)
        : Value{Type::ARRAY}
    {
        _array = new Array{std::move(array)};
    }


    ~ArrayValue()
    {
        delete _array;
    }
};


struct BooleanValue final : public Value
{
    explicit BooleanValue() : Value(Type::BOOLEAN)
    {
        _boolean = {};
    }


    explicit BooleanValue(bool boolean) : Value(Type::BOOLEAN)
    {
        _boolean = boolean;
    }
};


struct FloatValue final : public Value
{
    explicit FloatValue() : Value(Type::FLOAT)
    {
        _float = {};
    }


    explicit FloatValue(double number) : Value(Type::FLOAT)
    {
        _float = number;
    }
};


struct IntegerValue final : public Value
{
    explicit IntegerValue() : Value(Type::INTEGER)
    {
        _integer = {};
    }


    explicit IntegerValue(int64_t integer) : Value(Type::INTEGER)
    {
        _integer = integer;
    }
};


struct StringValue final : public Value
{
    explicit StringValue() : Value(Type::STRING)
    {
        _string = new std::string;
    }


    explicit StringValue(const std::string &str) : Value(Type::STRING)
    {
        _string = new std::string{str};
    }


    explicit StringValue(std::string &&str) : Value(Type::STRING)
    {
        _string = new std::string{std::move(str)};
    }


    ~StringValue()
    {
        delete _string;
    }
};


struct LocalDateValue final : public Value
{
    explicit LocalDateValue() : Value(Type::LOCAL_DATE)
    {
        _local_date = {};
    }


    explicit LocalDateValue(const LocalDate &date) : Value(Type::LOCAL_DATE)
    {
        _local_date = date;
    }


    explicit LocalDateValue(LocalDate &&date) : Value(Type::LOCAL_DATE)
    {
        _local_date = std::move(date);
    }
};


struct LocalDateTimeValue final : public Value
{
    explicit LocalDateTimeValue() : Value(Type::LOCAL_DATETIME)
    {
        _local_datetime = {};
    }


    explicit LocalDateTimeValue(const LocalDateTime &datetime) : Value(Type::LOCAL_DATETIME)
    {
        _local_datetime = datetime;
    }


    explicit LocalDateTimeValue(LocalDateTime &&datetime) : Value(Type::LOCAL_DATETIME)
    {
        _local_datetime = std::move(datetime);
    }
};


struct LocalTimeValue final : public Value
{
    explicit LocalTimeValue() : Value(Type::LOCAL_TIME)
    {
        _local_time = {};
    }


    explicit LocalTimeValue(const LocalTime &time) : Value(Type::LOCAL_TIME)
    {
        _local_time = time;
    }


    explicit LocalTimeValue(LocalTime &&time) : Value(Type::LOCAL_TIME)
    {
        _local_time = std::move(time);
    }
};


struct OffsetDateTimeValue final : public Value
{
    explicit OffsetDateTimeValue() : Value(Type::OFFSET_DATETIME)
    {
        _offset_datetime = {};
    }


    explicit OffsetDateTimeValue(const OffsetDateTime &datetime) : Value(Type::OFFSET_DATETIME)
    {
        _offset_datetime = datetime;
    }


    explicit OffsetDateTimeValue(OffsetDateTime &&datetime) : Value(Type::OFFSET_DATETIME)
    {
        _offset_datetime = std::move(datetime);
    }
};


struct TableValue final : public Value
{
    explicit TableValue() : Value(Type::TABLE)
    {
        _table = new Table;
    }


    explicit TableValue(const Table &table) : Value(Type::TABLE)
    {
        _table = new Table{table};
    }


    explicit TableValue(Table &&table) : Value(Type::TABLE)
    {
        _table = new Table{std::move(table)};
    }


    ~TableValue()
    {
        delete _table;
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
        : type{v->type()}
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
