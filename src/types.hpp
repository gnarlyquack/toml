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

using Array = std::vector<Value>;
using LocalDate = date::year_month_day;
using LocalDateTime = date::local_time<std::chrono::microseconds>;
using LocalTime = date::time_of_day<std::chrono::microseconds>;
using OffsetDateTime = date::sys_time<std::chrono::microseconds>;
using Table = std::unordered_map<std::string, Value>;


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


    explicit Value(const Array &value)
        : _type(Type::ARRAY)
        , _array(new Array(value))
    {
    }


    explicit Value(Array &&value)
        : _type(Type::ARRAY)
        , _array(new Array(std::move(value)))
    {
    }


    explicit Value(bool value)
        : _type(Type::BOOLEAN)
        , _boolean(value)
    {
    }


    explicit Value(double value)
        : _type(Type::FLOAT)
        , _float(value)
    {
    }


    explicit Value(int64_t value)
        : _type(Type::INTEGER)
        , _integer(value)
    {
    }


    explicit Value(const std::string &value)
        : _type(Type::STRING)
        , _string(new std::string(value))
    {
    }


    explicit Value(std::string &&value)
        : _type(Type::STRING)
        , _string(new std::string(std::move(value)))
    {
    }


    explicit Value(const LocalDate &value)
        : _type(Type::LOCAL_DATE)
        , _local_date(value)
    {
    }


    explicit Value(const LocalDateTime &value)
        : _type(Type::LOCAL_DATETIME)
        , _local_datetime(value)
    {
    }


    explicit Value(const LocalTime &value)
        : _type(Type::LOCAL_TIME)
        , _local_time(value)
    {
    }


    explicit Value(const OffsetDateTime &value)
        : _type(Type::OFFSET_DATETIME)
        , _offset_datetime(value)
    {
    }


    explicit Value(const Table &value)
        : _type(Type::TABLE)
        , _table(new Table(value))
    {
    }


    explicit Value(Table &&value)
        : _type(Type::TABLE)
        , _table(new Table(std::move(value)))
    {
    }


    Value(const Value &other)
        : _type(other._type)
    {
        switch(_type)
        {
            case Type::ARRAY:
            {
                _array = new Array(*other._array);
            } break;

            case Type::BOOLEAN:
            {
                _boolean = other._boolean;
            } break;

            case Type::FLOAT:
            {
                _float = other._float;
            } break;

            case Type::INTEGER:
            {
                _integer = other._integer;
            } break;

            case Type::LOCAL_DATE:
            {
                _local_date = other._local_date;
            } break;

            case Type::LOCAL_DATETIME:
            {
                 new (&_local_datetime) LocalDateTime(other._local_datetime);
            } break;

            case Type::LOCAL_TIME:
            {
                _local_time = other._local_time;
            } break;

            case Type::OFFSET_DATETIME:
            {
                new (&_offset_datetime) OffsetDateTime(other._offset_datetime);
            } break;

            case Type::STRING:
            {
                _string = new std::string(*other._string);
            } break;

            case Type::TABLE:
            {
                _table = new Table(*other._table);
            } break;

            default:
            {
                assert(_type == Type::INVALID);
            } break;
        }
    }


    Value(Value &&other)
        : _type(other._type)
    {
        take(other);
    }


    ~Value()
    {
        destroy();
    }


    Value &
    operator=(const Value &other)
    {
        if (this != &other)
        {
            Value temp(other);
            destroy();
            take(temp);
        }
        return *this;
    }


    Value &
    operator=(Value &&other)
    {
        if (this != &other)
        {
            destroy();
            take(other);
        }
        return *this;
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


    void
    take(Value &other)
    {
        switch(other._type)
        {
            case Type::ARRAY:
            {
                _array = other._array;
            } break;

            case Type::BOOLEAN:
            {
                _boolean = other._boolean;
            } break;

            case Type::FLOAT:
            {
                _float = other._float;
            } break;

            case Type::INTEGER:
            {
                _integer = other._integer;
            } break;

            case Type::LOCAL_DATE:
            {
                _local_date = other._local_date;
            } break;

            case Type::LOCAL_DATETIME:
            {
                new (&_local_datetime) LocalDateTime(other._local_datetime);
                other._local_datetime.~LocalDateTime();
            } break;

            case Type::LOCAL_TIME:
            {
                _local_time = other._local_time;
            } break;

            case Type::OFFSET_DATETIME:
            {
                new (&_offset_datetime) OffsetDateTime(other._offset_datetime);
                other._offset_datetime.~OffsetDateTime();
            } break;

            case Type::STRING:
            {
                _string = other._string;
            } break;

            case Type::TABLE:
            {
                _table = other._table;
            } break;

            default:
            {
                assert(other._type == Type::INVALID);
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
            case Type::ARRAY:
            {
                delete _array;
            } break;

            case Type::LOCAL_DATETIME:
            {
                _local_datetime.~LocalDateTime();
            } break;

            case Type::OFFSET_DATETIME:
            {
                _offset_datetime.~OffsetDateTime();
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
        Value value;
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

    Record(Value v, uint32_t l, uint32_t c)
        : type(v.type())
        , line(l)
        , column(c)
        , value(v)
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
