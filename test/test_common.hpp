//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TEST_COMMON_HPP
#define TOML_TEST_COMMON_HPP

#include <lexer.hpp>
#include <parser.hpp>

#include <gtest/gtest.h>

//#include <cmath>

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
        (left.line == right.line)
        && (left.column == right.column)
        && (left.message == right.message);
    return result;
}


inline bool
operator==(const Array &left, const Array &right);

inline bool
operator==(const Table &left, const Table &right);


inline bool
operator==(const Value &left, const Value &right)
{
    bool result = false;
    if (left.type() == right.type())
    {
        switch (left.type())
        {
            case Value::Type::ARRAY:
            {
                result = left.as_array() == right.as_array();
            } break;

            case Value::Type::BOOLEAN:
            {
                result = left.as_boolean() == right.as_boolean();
            } break;

            case Value::Type::FLOAT:
            {
                f64 l = left.as_float();
                f64 r = right.as_float();
                result = (l == r) || (std::isnan(l) && std::isnan(r));
            } break;

            case Value::Type::INTEGER:
            {
                result = left.as_integer() == right.as_integer();
            } break;

            case Value::Type::LOCAL_DATE:
            {
                result = left.as_local_date() == right.as_local_date();
            } break;

            case Value::Type::LOCAL_DATETIME:
            {
                result = left.as_local_datetime() == right.as_local_datetime();
            } break;

            case Value::Type::LOCAL_TIME:
            {
                result = left.as_local_time() == right.as_local_time();
            } break;

            case Value::Type::OFFSET_DATETIME:
            {
                result = left.as_offset_datetime() == right.as_offset_datetime();
            } break;

            case Value::Type::STRING:
            {
                result = left.as_string() == right.as_string();
            } break;

            case Value::Type::TABLE:
            {
                result = left.as_table() == right.as_table();
            } break;

            case Value::Type::INVALID:
            {
                assert(false);
            } break;
        }
    }
    return result;
}


inline bool
operator==(const Array &left, const Array &right)
{
    bool result = left.size() == right.size();

    for (size_t i = 0; result && i < left.size(); ++i)
    {
        Value *lvalue = left[i];
        Value *rvalue = right[i];
        result = *lvalue == *rvalue;
    }

    return result;
}


inline bool
operator==(const Table &left, const Table &right)
{
    bool result = left.size() == right.size();
    if (result)
    {
        for (auto i : left)
        {
            result = (right.find(i.first) != right.end())
                && (*i.second == *right.at(i.first));
            if (!result)
            {
                break;
            }
        }
    }

    return result;
}


inline bool
operator==(const Token &left, const Token &right)
{
    bool result =
        (left.type == right.type)
        && (left.position == right.position)
        && (left.line == right.line)
        && (left.column == right.column)
        && (left.lexeme == right.lexeme);
    if (result && (left.type == TOKEN_VALUE))
    {
        result = *left.value == *right.value;
    }
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
            os << "LocalTime(" << value.as_local_datetime() << ')';
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
operator<<(std::ostream &os, const Token &token)
{
    if (token.type == TOKEN_VALUE)
    {
        os << "VALUE("
            << *token.value << ", "
            << token.lexeme << ", "
            << token.position << ", "
            << token.line << ", "
            << token.column  << ")";
    }
    else
    {
        os << TOKEN_NAMES[token.type] << '('
            << token.lexeme << ", "
            << token.position << ", "
            << token.line << ", "
            << token.column  << ")";
    }

    return os;
}


inline std::ostream &
operator<<(std::ostream &os, const Error &error)
{
    os << "Error(" << error.line << ", " << error.column << ", \"" << error.message << "\")";
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
