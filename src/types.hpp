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


struct Value final
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


    explicit Value()
        : _type(Type::INVALID)
    {
    }


    ~Value()
    {
        switch (_type)
        {
            case Type::ARRAY:
            {
                delete _array;
            } break;

            case Type::STRING:
            {
                delete _string;
            } break;

            case Type::TABLE:
            {
                delete _table;
            } break;

            default:
            {
                // do nothing
            } break;
        }
    }


    static Value *
    of_array()
    {
        auto result = new Value(Type::ARRAY);
        result->_array = new Array;
        return result;
    }


    static Value *
    of_array(const Array &value)
    {
        auto result = new Value(Type::ARRAY);
        result->_array = new Array(value);
        return result;
    }


    static Value *
    of_array(Array &&value)
    {
        auto result = new Value(Type::ARRAY);
        result->_array = new Array(std::move(value));
        return result;
    }


    static Value *
    of_boolean()
    {
        auto result = new Value(Type::BOOLEAN);
        result->_boolean = {};
        return result;
    }


    static Value *
    of_boolean(bool value)
    {
        auto result = new Value(Type::BOOLEAN);
        result->_boolean = value;
        return result;
    }


    static Value *
    of_float()
    {
        auto result = new Value(Type::FLOAT);
        result->_float = {};
        return result;
    }


    static Value *
    of_float(double value)
    {
        auto result = new Value(Type::FLOAT);
        result->_float = value;
        return result;
    }


    static Value *
    of_integer()
    {
        auto result = new Value(Type::INTEGER);
        result->_integer = {};
        return result;
    }


    static Value *
    of_integer(int64_t value)
    {
        auto result = new Value(Type::INTEGER);
        result->_integer = value;
        return result;
    }


    static Value *
    of_string()
    {
        auto result = new Value(Type::STRING);
        result->_string = new std::string;
        return result;
    }


    static Value *
    of_string(const std::string &value)
    {
        auto result = new Value(Type::STRING);
        result->_string = new std::string(value);
        return result;
    }


    static Value *
    of_string(std::string &&value)
    {
        auto result = new Value(Type::STRING);
        result->_string = new std::string(std::move(value));
        return result;
    }


    static Value *
    of_local_date()
    {
        auto result = new Value(Type::LOCAL_DATE);
        result->_local_date = {};
        return result;
    }


    static Value *
    of_local_date(const LocalDate &value)
    {
        auto result = new Value(Type::LOCAL_DATE);
        result->_local_date = value;
        return result;
    }


    static Value *
    of_local_date(LocalDate &&value)
    {
        auto result = new Value(Type::LOCAL_DATE);
        result->_local_date = std::move(value);
        return result;
    }


    static Value *
    of_local_datetime()
    {
        auto result = new Value(Type::LOCAL_DATETIME);
        result->_local_datetime = {};
        return result;
    }


    static Value *
    of_local_datetime(const LocalDateTime &value)
    {
        auto result = new Value(Type::LOCAL_DATETIME);
        result->_local_datetime = value;
        return result;
    }


    static Value *
    of_local_datetime(LocalDateTime &&value)
    {
        auto result = new Value(Type::LOCAL_DATETIME);
        result->_local_datetime = std::move(value);
        return result;
    }


    static Value *
    of_local_time()
    {
        auto result = new Value(Type::LOCAL_TIME);
        result->_local_time = {};
        return result;
    }


    static Value *
    of_local_time(const LocalTime &value)
    {
        auto result = new Value(Type::LOCAL_TIME);
        result->_local_time = value;
        return result;
    }


    static Value *
    of_local_time(LocalTime &&value)
    {
        auto result = new Value(Type::LOCAL_TIME);
        result->_local_time = std::move(value);
        return result;
    }


    static Value *
    of_offset_datetime()
    {
        auto result = new Value(Type::OFFSET_DATETIME);
        result->_offset_datetime = {};
        return result;
    }



    static Value *
    of_offset_datetime(const OffsetDateTime &value)
    {
        auto result = new Value(Type::OFFSET_DATETIME);
        result->_offset_datetime = value;
        return result;
    }


    static Value *
    of_offset_datetime(OffsetDateTime &&value)
    {
        auto result = new Value(Type::OFFSET_DATETIME);
        result->_offset_datetime = std::move(value);
        return result;
    }


    static Value *
    of_table()
    {
        auto result = new Value(Type::TABLE);
        result->_table = new Table;
        return result;
    }



    static Value *
    of_table(const Table &value)
    {
        auto result = new Value(Type::TABLE);
        result->_table = new Table(value);
        return result;
    }


    static Value *
    of_table(Table &&value)
    {
        auto result = new Value(Type::TABLE);
        result->_table = new Table(std::move(value));
        return result;
    }


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


private:
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
