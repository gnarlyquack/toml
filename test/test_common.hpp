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
    if (left.type == right.type)
    {
        switch (left.type)
        {
            case TYPE_ARRAY:
            {
                result = static_cast<const ArrayValue &>(left).value
                    == static_cast<const ArrayValue &>(right).value;
            } break;

            case TYPE_BOOL:
            {
                result = static_cast<const BooleanValue &>(left).value
                    == static_cast<const BooleanValue &>(right).value;
            } break;

            case TYPE_FLOAT:
            {
                f64 l = static_cast<const FloatValue &>(left).value;
                f64 r = static_cast<const FloatValue &>(right).value;
                //std::cout << "Comparing floats: " << l << " == " << r << '\n';
                result = (l == r) || (std::isnan(l) && std::isnan(r));
            } break;

            case TYPE_INTEGER:
            {
                result = static_cast<const IntegerValue &>(left).value
                    == static_cast<const IntegerValue &>(right).value;
            } break;

            case TYPE_LOCAL_DATE:
            {
                result = static_cast<const LocalDateValue &>(left).value
                    == static_cast<const LocalDateValue &>(right).value;
            } break;

            case TYPE_LOCAL_DATETIME:
            {
                result = static_cast<const LocalDateTimeValue &>(left).value
                    == static_cast<const LocalDateTimeValue &>(right).value;
            } break;

            case TYPE_LOCAL_TIME:
            {
                result = static_cast<const LocalTimeValue &>(left).value
                    == static_cast<const LocalTimeValue &>(right).value;
            } break;

            case TYPE_OFFSET_DATETIME:
            {
                result = static_cast<const OffsetDateTimeValue &>(left).value
                    == static_cast<const OffsetDateTimeValue &>(right).value;
            } break;

            case TYPE_STRING:
            {
                result = static_cast<const StringValue &>(left).value
                    == static_cast<const StringValue &>(right).value;
            } break;

            case TYPE_TABLE:
            {
                result = static_cast<const TableValue &>(left).value
                    == static_cast<const TableValue &>(right).value;
            } break;

            case TYPE_INVALID:
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
    switch (value.type)
    {
        case TYPE_ARRAY:
        {
            os << "Array(" << static_cast<const ArrayValue &>(value).value << ')';
        } break;

        case TYPE_BOOL:
        {
            os << "Boolean(" << std::boolalpha << static_cast<const BooleanValue &>(value).value << ')';
        } break;

        case TYPE_FLOAT:
        {
            os << "Float(" << static_cast<const FloatValue &>(value).value << ')';
        } break;

        case TYPE_INTEGER:
        {
            os << "Integer(" << static_cast<const IntegerValue &>(value).value << ')';
        } break;

        case TYPE_LOCAL_DATE:
        {
            os << "LocalDate(" << static_cast<const LocalDateValue &>(value).value << ')';
        } break;

        case TYPE_LOCAL_DATETIME:
        {
            os << "LocalDateTime(" << static_cast<const LocalDateTimeValue &>(value).value << ')';
        } break;

        case TYPE_LOCAL_TIME:
        {
            os << "LocalTime(" << static_cast<const LocalTimeValue &>(value).value << ')';
        } break;

        case TYPE_OFFSET_DATETIME:
        {
            os << "OffsetDateTime(" << date::format("%Y-%m-%d %T %Z", static_cast<const OffsetDateTimeValue &>(value).value) << ')';
        } break;

        case TYPE_STRING:
        {
            os << "String(" << static_cast<const StringValue &>(value).value << ')';
        } break;

        case TYPE_TABLE:
        {
            os << "Table(" << static_cast<const TableValue &>(value).value << ')';
        } break;

        case TYPE_INVALID:
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
