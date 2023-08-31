//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_PARSER_HPP
#define TOML_PARSER_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "error.hpp"
#include "types.hpp"


namespace toml
{


bool
parse_toml(const std::string &toml, Table &result, std::vector<Error> &errors);


bool
parse_with_metadata(const std::string &toml, Definitions &definitions, std::vector<Error> &errors);


} // namespace toml


#endif // TOML_PARSER_HPP
