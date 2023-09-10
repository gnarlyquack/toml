//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TYPES_HPP
#define TOML_TYPES_HPP

#include <date/tz.h>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility> // move
#include <vector>


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
        , _local_time(new LocalTime(value))
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
                _local_time = new LocalTime(*other._local_time);
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


    LocalDate
    as_local_date() const
    {
        assert(_type == Type::LOCAL_DATE);
        return _local_date;
    }


    LocalDateTime
    as_local_datetime() const
    {
        assert(_type == Type::LOCAL_DATETIME);
        return _local_datetime;
    }


    LocalTime &
    as_local_time()
    {
        assert(_type == Type::LOCAL_TIME);
        return *_local_time;
    }


    const LocalTime &
    as_local_time() const
    {
        assert(_type == Type::LOCAL_TIME);
        return *_local_time;
    }


    OffsetDateTime
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
        LocalTime *_local_time;
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

            case Type::LOCAL_TIME:
            {
                delete _local_time;
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


struct Key
{
    std::string value;
    uint32_t line;
    uint32_t column;
};


inline bool
operator==(const Key &left, const Key &right)
{
    bool result = left.value == right.value;
    return result;
}


struct KeyHasher
{
    size_t operator()(const toml::Key& key) const
    {
        return std::hash<std::string>()(key.value);
    }
};


struct Definition;

using DefinitionTable = std::unordered_map<Key, Definition, KeyHasher>;
using DefinitionArray = std::vector<Definition>;

// In order to access common union subsequence member '_type' in Definition,
// all union members must be standard layout.
static_assert(std::is_standard_layout<DefinitionTable>::value, "toml::DefinitionTable is not standard layout.");
static_assert(std::is_standard_layout<DefinitionArray>::value, "toml::DefinitionArray is not standard layout.");
static_assert(std::is_standard_layout<Value>::value, "toml::Value is not standard layout.");


struct Definition
{
    Definition()
        : _type(Value::Type::INVALID)
        , _line(0)
        , _column(0)
    {
    }


    Definition(Value::Type type, uint32_t line, uint32_t column)
        : _type(type)
        , _line(line)
        , _column(column)
    {
        switch (_type)
        {
            case Value::Type::ARRAY:
            {
                _array.value = new DefinitionArray;
            } break;

            case Value::Type::TABLE:
            {
                _table.value = new DefinitionTable;
            } break;

            default:
            {
                assert(false);
            } break;
        }
    }


    Definition(const DefinitionArray &value, uint32_t line, uint32_t column)
        : _array{Value::Type::ARRAY, new DefinitionArray(value)}
        , _line(line)
        , _column(column)
    {
    }


    Definition(DefinitionArray &&value, uint32_t line, uint32_t column)
        : _array{Value::Type::ARRAY, new DefinitionArray(std::move(value))}
        , _line(line)
        , _column(column)
    {
    }


    Definition(const DefinitionTable &value, uint32_t line, uint32_t column)
        : _table{Value::Type::TABLE, new DefinitionTable(value)}
        , _line(line)
        , _column(column)
    {
    }


    Definition(DefinitionTable &&value, uint32_t line, uint32_t column)
        : _table{Value::Type::TABLE, new DefinitionTable(std::move(value))}
        , _line(line)
        , _column(column)
    {
    }


    Definition(const Value &value, uint32_t line, uint32_t column)
        : _value(value)
        , _line(line)
        , _column(column)
    {
        assert((_type != Value::Type::ARRAY) || (_type != Value::Type::TABLE));
    }


    Definition(Value &&value, uint32_t line, uint32_t column)
        : _value(std::move(value))
        , _line(line)
        , _column(column)
    {
        assert((_type != Value::Type::ARRAY) || (_type != Value::Type::TABLE));
    }


    Definition(const Definition &other)
        : _line(other._line)
        , _column(other._column)
    {
        switch (other._type)
        {
            case Value::Type::ARRAY:
            {
                _array = { other._type, new DefinitionArray(*other._array.value) };
            } break;

            case Value::Type::TABLE:
            {
                _table = { other._type, new DefinitionTable(*other._table.value) };
            } break;

            default:
            {
                new (&_value) Value(other._value);
            } break;
        }
    }


    Definition(Definition &&other)
        : _line(other._line)
        , _column(other._column)
    {
        take(other);
    }


    ~Definition()
    {
        destroy();
    }


    Definition &
    operator=(const Definition &other)
    {
        if (this != &other)
        {
            Definition temp(other);
            destroy();
            take(temp);
        }
        return *this;
    }


    Definition &
    operator=(Definition &&other)
    {
        if (this != &other)
        {
            destroy();
            take(other);
        }
        return *this;
    }


    Value::Type
    type() const
    {
        return _type;
    }


    DefinitionArray &
    as_array()
    {
        assert(_type == Value::Type::ARRAY);
        return *_array.value;
    }


    const DefinitionArray &
    as_array() const
    {
        assert(_type == Value::Type::ARRAY);
        return *_array.value;
    }


    DefinitionTable &
    as_table()
    {
        assert(_type == Value::Type::TABLE);
        return *_table.value;
    }


    const DefinitionTable &
    as_table() const
    {
        assert(_type == Value::Type::TABLE);
        return *_table.value;
    }


    Value &
    as_value()
    {
        assert((_type != Value::Type::ARRAY) && (_type != Value::Type::TABLE));
        return _value;
    }


    const Value &
    as_value() const
    {
        assert((_type != Value::Type::ARRAY) && (_type != Value::Type::TABLE));
        return _value;
    }


    bool
    as_boolean() const
    {
        return _value.as_boolean();
    }


    double
    as_float() const
    {
        return _value.as_float();
    }


    int64_t
    as_integer() const
    {
        return _value.as_integer();
    }


    std::string &
    as_string()
    {
        return _value.as_string();
    }


    const std::string &
    as_string() const
    {
        return _value.as_string();
    }


    LocalDate
    as_local_date() const
    {
        return _value.as_local_date();
    }


    LocalDateTime
    as_local_datetime() const
    {
        return _value.as_local_datetime();
    }


    LocalTime &
    as_local_time()
    {
        return _value.as_local_time();
    }


    const LocalTime &
    as_local_time() const
    {
        return _value.as_local_time();
    }


    OffsetDateTime
    as_offset_datetime() const
    {
        return _value.as_offset_datetime();
    }


    uint32_t
    line() const
    {
        return _line;
    };


    uint32_t
    column() const
    {
        return _column;
    };


private:

    union
    {
        Value::Type _type;
        Value _value;
        struct {
            Value::Type type;
            DefinitionTable *value;
        } _table;
        struct {
            Value::Type type;
            DefinitionArray *value;
        } _array;
    };

    uint32_t _line;

    uint32_t _column;


    void
    take(Definition &other)
    {
        switch (other._type)
        {
            case Value::Type::ARRAY:
            {
                _array = { other._type, other._array.value };
                other._type = Value::Type::INVALID;
            } break;

            case Value::Type::TABLE:
            {
                _table = { other._type, other._table.value };
                other._type = Value::Type::INVALID;
            } break;

            default:
            {
                new (&_value) Value(std::move(other._value));
                assert(other._value.type() == Value::Type::INVALID);
            } break;
        }

    }


    void
    destroy()
    {
        switch(_type)
        {
            case Value::Type::ARRAY:
            {
                delete _array.value;
            } break;

            case Value::Type::TABLE:
            {
                delete _table.value;
            } break;

            default:
            {
                _value.~Value();
            } break;
        }
    }
};


} // namespace toml


#endif // TOML_TYPES_HPP
