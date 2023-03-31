//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TEST_COMMON_HPP
#define TOML_TEST_COMMON_HPP

#include <cmath>

#include "lexer.hpp"
#include "parser.hpp"


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
operator==(const Table &left, const Table &right);


inline bool
operator==(const Value &left, const Value &right)
{
    bool result = false;
    if (left.type == right.type)
    {
        switch (left.type)
        {
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
operator<<(std::ostream &os, const Table &value);


inline std::ostream &
operator<<(std::ostream &os, const Value &value)
{
    switch (value.type)
    {
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

    os << ")";
    return os;
}


inline std::ostream &
operator<<(std::ostream &os, const Value *value)
{
    os << *value;
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
        os << "(\"" << i.first << "\", " << *i.second << ')';
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
            << token.line << ", "
            << token.column  << ")";
    }
    else
    {
        os << TOKEN_NAMES[token.type] << '('
            << token.lexeme << ", "
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


#endif // TOML_TEST_COMMON_HPP