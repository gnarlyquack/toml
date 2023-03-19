//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_LEXER_HPP
#define TOML_LEXER_HPP

#include <string>
#include <vector>

#include "error.hpp"
#include "token.hpp"


namespace toml
{


bool
lex_toml(const std::string &toml, std::vector<Token> &tokens, std::vector<Error> &errors);


} // namespace toml


#endif // TOML_LEXER_HPP
