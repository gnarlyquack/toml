// This file is part of toml. It is subject to the license terms in the LICENSE
// file found in the top-level directory of this distribution. No part of this
// project, including this file, may be copied, modified, propagated, or
// distributed except according to the terms contained in the LICENSE file.

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


struct SourceLocation
{
    uint64_t index;
    uint32_t line;
    uint32_t column;
};


struct Key final
{
    std::string value;
    SourceLocation location;
};


inline bool
operator==(const Key &lhs, const Key &rhs)
{
    bool result = lhs.value == rhs.value;
    return result;
}


struct KeyHasher
{
    size_t operator()(const toml::Key &key) const
    {
        return std::hash<std::string>()(key.value);
    }
};


struct Definition;
using DefinitionArray = std::vector<Definition>;

// Thin wrapper around std::unordered_map to allow key lookup with strings
struct DefinitionTable final
{

private:

    using DefinitionTableImpl = std::unordered_map<Key, Definition, KeyHasher>;


public:

    using key_type = Key;
    using mapped_type = Definition;
    using value_type = std::pair<const Key, Definition>;
    using size_type = DefinitionTableImpl::size_type;
    using difference_type = DefinitionTableImpl::difference_type;
    using iterator = DefinitionTableImpl::iterator;
    using const_iterator = DefinitionTableImpl::const_iterator;


    //
    // Iterators
    //

    iterator
    begin() noexcept
    {
        return _table.begin();
    }


    const_iterator
    begin() const noexcept
    {
        return _table.begin();
    }


    const_iterator
    cbegin() const noexcept
    {
        return _table.cbegin();
    }


    iterator
    end() noexcept
    {
        return _table.end();
    }


    const_iterator
    end() const noexcept
    {
        return _table.end();
    }


    const_iterator
    cend() const noexcept
    {
        return _table.cend();
    }


    //
    // Capacity
    //

    bool
    empty() const noexcept
    {
        return _table.empty();
    }


    size_type
    size() const noexcept
    {
        return _table.size();
    }


    //
    // Modifiers
    //

    iterator
    erase(iterator pos)
    {
        return _table.erase(pos);
    }


    iterator
    erase(const_iterator pos)
    {
        return _table.erase(pos);
    }


    size_type
    erase(const Key& key)
    {
        return _table.erase(key);
    }


    size_type
    erase(const std::string& key)
    {
        return _table.erase({ key, {} });
    }


    //
    // Lookup
    //

    Definition&
    at(const Key& key)
    {
        return _table.at(key);
    }


    const Definition&
    at(const Key& key) const
    {
        return _table.at(key);
    }


    Definition&
    at(const std::string& key)
    {
        return _table.at({ key, {} });
    }


    const Definition&
    at(const std::string& key) const
    {
        return _table.at({ key, {} });
    }


    Definition&
    operator[](const Key& key)
    {
        return _table[key];
    }


    Definition&
    operator[](Key&& key)
    {
        return _table[std::move(key)];
    }


    iterator
    find(const Key& key)
    {
        return _table.find(key);
    }


    const_iterator
    find(const Key& key) const
    {
        return _table.find(key);
    }


    iterator
    find(const std::string& key)
    {
        return _table.find({ key, {} });
    }


    const_iterator
    find(const std::string& key) const
    {
        return _table.find({ key, {} });
    }


    friend bool
    operator==(const DefinitionTable& lhs, const DefinitionTable& rhs);


    friend bool
    operator!=(const DefinitionTable& lhs, const DefinitionTable& rhs);


private:

    DefinitionTableImpl _table;
};


// In order to access common union subsequence member '_type' in Definition,
// all union members must be standard layout.
static_assert(std::is_standard_layout<DefinitionTable>::value, "toml::DefinitionTable is not standard layout.");
static_assert(std::is_standard_layout<DefinitionArray>::value, "toml::DefinitionArray is not standard layout.");
static_assert(std::is_standard_layout<Value>::value, "toml::Value is not standard layout.");


struct Definition final
{
    Definition()
        : _type(Value::Type::INVALID)
        , _location()
    {
    }


    Definition(Value::Type type, const SourceLocation &location)
        : _type(type)
        , _location(location)
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


    Definition(const DefinitionArray &value, const SourceLocation &location)
        : _array{Value::Type::ARRAY, new DefinitionArray(value)}
        , _location(location)
    {
    }


    Definition(DefinitionArray &&value, const SourceLocation &location)
        : _array{Value::Type::ARRAY, new DefinitionArray(std::move(value))}
        , _location(location)
    {
    }


    Definition(const DefinitionTable &value, const SourceLocation &location)
        : _table{Value::Type::TABLE, new DefinitionTable(value)}
        , _location(location)
    {
    }


    Definition(DefinitionTable &&value, const SourceLocation &location)
        : _table{Value::Type::TABLE, new DefinitionTable(std::move(value))}
        , _location(location)
    {
    }


    Definition(const Value &value, const SourceLocation &location)
        : _value(value)
        , _location(location)
    {
        assert((_type != Value::Type::ARRAY) || (_type != Value::Type::TABLE));
    }


    Definition(Value &&value, const SourceLocation &location)
        : _value(std::move(value))
        , _location(location)
    {
        assert((_type != Value::Type::ARRAY) || (_type != Value::Type::TABLE));
    }


    Definition(const Definition &other)
        : _location(other._location)
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
        : _location(std::move(other._location))
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


    const SourceLocation &
    location() const
    {
        return _location;
    }


    uint64_t
    index() const
    {
        return _location.index;
    }


    uint32_t
    line() const
    {
        return _location.line;
    };


    uint32_t
    column() const
    {
        return _location.column;
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

    SourceLocation _location;


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


inline bool
operator==(const Value &lhs, const Value &rhs)
{
    bool result = false;
    if (lhs.type() == rhs.type())
    {
        switch (lhs.type())
        {
            case Value::Type::ARRAY:
            {
                result = lhs.as_array() == rhs.as_array();
            } break;

            case Value::Type::BOOLEAN:
            {
                result = lhs.as_boolean() == rhs.as_boolean();
            } break;

            case Value::Type::FLOAT:
            {
                double l = lhs.as_float();
                double r = rhs.as_float();
                result = (l == r) || (std::isnan(l) && std::isnan(r));
            } break;

            case Value::Type::INTEGER:
            {
                result = lhs.as_integer() == rhs.as_integer();
            } break;

            case Value::Type::LOCAL_DATE:
            {
                result = lhs.as_local_date() == rhs.as_local_date();
            } break;

            case Value::Type::LOCAL_DATETIME:
            {
                result = lhs.as_local_datetime() == rhs.as_local_datetime();
            } break;

            case Value::Type::LOCAL_TIME:
            {
                result = lhs.as_local_time().to_duration() == rhs.as_local_time().to_duration();
            } break;

            case Value::Type::OFFSET_DATETIME:
            {
                result = lhs.as_offset_datetime() == rhs.as_offset_datetime();
            } break;

            case Value::Type::STRING:
            {
                result = lhs.as_string() == rhs.as_string();
            } break;

            case Value::Type::TABLE:
            {
                result = lhs.as_table() == rhs.as_table();
            } break;

            case Value::Type::INVALID:
            {
                result = false;
            } break;
        }
    }
    return result;
}


inline bool
operator!=(const Value &lhs, const Value &rhs)
{
    bool not_result = lhs == rhs;
    return !not_result;
}


inline bool
operator==(const Definition& lhs, const Definition& rhs)
{
    bool result = false;
    if (lhs.type() == rhs.type())
    {
        switch (lhs.type())
        {
            case Value::Type::ARRAY:
            {
                result = lhs.as_array() == rhs.as_array();
            } break;

            case Value::Type::TABLE:
            {
                result = lhs.as_table() == rhs.as_table();
            } break;

            default:
            {
                result = lhs.as_value() == rhs.as_value();
            } break;
        }
    }

    return result;
}


inline bool
operator!=(const Definition& lhs, const Definition& rhs)
{
    bool not_result = lhs == rhs;
    return !not_result;
}


inline bool
operator==(const DefinitionTable& lhs, const DefinitionTable& rhs)
{
    return lhs._table == rhs._table;
}


inline bool
operator!=(const DefinitionTable& lhs, const DefinitionTable& rhs)
{
    return lhs._table != rhs._table;
}


struct Error
{
    SourceLocation location;
    std::string message;
};


} // namespace toml


#endif // TOML_TYPES_HPP
