//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_ERROR_HPP
#define TOML_ERROR_HPP

#include <string>


namespace toml
{


struct Error
{
    std::string::size_type line;
    std::string::size_type column;
    std::string message;
};


}

#endif // TOML_ERROR_HPP
