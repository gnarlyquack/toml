//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TEST_COMMON_HPP
#define TOML_TEST_COMMON_HPP


#include <parser.hpp>

#include <gtest/gtest.h>


namespace toml
{


constexpr const char *TOKEN_NAMES[] = {
    "ERROR",
    "COMMA",
    "DAY",
    "DOUBLE LBRACKET",
    "DOUBLE RBRACKET",
    "EOF",
    "FRACTION",
    "KEY",
    "HOUR",
    "LBRACE",
    "LBRACKET",
    "MINUS",
    "MINUTE",
    "MONTH",
    "NEWLINE",
    "PLUS",
    "RBRACE",
    "RBRACKET",
    "SECOND",
    "VALUE",
    "YEAR",
};


inline bool
operator==(const Error &left, const Error &right)
{
    bool result =
           (left.location.index == right.location.index)
        && (left.location.line == right.location.line)
        && (left.location.column == right.location.column)
        && (left.message == right.message);
    return result;
}


inline std::ostream &
operator<<(std::ostream &os, const Array &value);


inline std::ostream &
operator<<(std::ostream &os, const Table &value);


inline std::ostream &
operator<<(std::ostream &os, const Value &value)
{
    switch (value.type())
    {
        case Value::Type::ARRAY:
        {
            os << "Array(" << value.as_array() << ')';
        } break;

        case Value::Type::BOOLEAN:
        {
            os << "Boolean(" << std::boolalpha << value.as_boolean() << ')';
        } break;

        case Value::Type::FLOAT:
        {
            os << "Float(" << value.as_float() << ')';
        } break;

        case Value::Type::INTEGER:
        {
            os << "Integer(" << value.as_integer() << ')';
        } break;

        case Value::Type::LOCAL_DATE:
        {
            os << "LocalDate(" << value.as_local_date() << ')';
        } break;

        case Value::Type::LOCAL_DATETIME:
        {
            os << "LocalDateTime(" << value.as_local_datetime() << ')';
        } break;

        case Value::Type::LOCAL_TIME:
        {
            os << "LocalTime(" << value.as_local_time() << ')';
        } break;

        case Value::Type::OFFSET_DATETIME:
        {
            os << "OffsetDateTime(" << date::format("%Y-%m-%d %T %Z", value.as_offset_datetime()) << ')';
        } break;

        case Value::Type::STRING:
        {
            os << "String(" << value.as_string() << ')';
        } break;

        case Value::Type::TABLE:
        {
            os << "Table(" << value.as_table() << ')';
        } break;

        case Value::Type::INVALID:
        {
            assert(false);
        } break;
    }

    return os;
}


inline std::ostream &
operator<<(std::ostream &os, const Value *value)
{
    os << *value;
    return os;
}


inline std::ostream &
operator<<(std::ostream &os, const Array &value)
{
    uint64_t count = 0;
    for (auto i : value)
    {
        if (count++)
        {
            os << ", ";
        }
        os << i;
    }
    return os;
}


inline std::ostream &
operator<<(std::ostream &os, const Table &value)
{
    os << "{ ";
    uint64_t count = 0;
    for (auto i : value)
    {
        if (count++)
        {
            os << ", ";
        }
        os << "(\"" << i.first << "\", " << i.second << ')';
    }
    os << " }";
    return os;
}


inline std::ostream &
operator<<(std::ostream &os, const Error &error)
{
    os << "Error(" << error.location.index << ", " << error.location.line << ", " << error.location.column
                   << ", \"" << error.message << "\")";
    return os;
}


} // namespace toml


inline void
assert_parsed(const std::string &toml, const toml::Table &expected)
{
    toml::Table actual;
    std::vector<toml::Error> errors;
    bool result = parse_toml(toml, actual, errors);

    EXPECT_TRUE(result);
    EXPECT_EQ(actual, expected);
    EXPECT_EQ(errors, std::vector<toml::Error>{});
}


inline void
assert_errors(const std::string &toml, const std::vector<toml::Error> &expected)
{
    toml::Table table;
    std::vector<toml::Error> actual;
    bool result = parse_toml(toml, table, actual);

    ASSERT_FALSE(result);
    ASSERT_EQ(actual, expected);
}


#endif // TOML_TEST_COMMON_HPP
