//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_TEST_COMMON_HPP
#define TOML_TEST_COMMON_HPP


namespace toml
{


inline bool
operator==(const Error &left, const Error &right)
{
    bool result =
        (left.line == right.line)
        && (left.column == right.column)
        && (left.message == right.message);
    return result;
}


} // namespace toml


#endif // TOML_TEST_COMMON_HPP
