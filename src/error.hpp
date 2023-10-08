//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef TOML_ERROR_HPP
#define TOML_ERROR_HPP

#include "types.hpp"
#include "parser.hpp"

#include <vector>


namespace toml
{


void
expected_end_of_line(Parser &parser);


void
incomplete_unicode_codepoint(Lexer &lexer, const SourceLocation &location, s32 expected, s32 actual);


void
invalid_unicode_codepoint(Lexer &lexer, const SourceLocation &location, u32 codepoint);


void
invalid_utf8_byte(Lexer &lexer, const SourceLocation &location, byte b);


void
overlong_utf8_encoding(Lexer &lexer, const SourceLocation &location, u32 codepoint, u32 expected, u32 actual);


void
year_wrong_length(Lexer &lexer, const SourceLocation &location);


void
month_wrong_length(Lexer &lexer, const SourceLocation &location);


void
day_wrong_length(Lexer &lexer, const SourceLocation &location);


void
hour_wrong_length(Lexer &lexer, const SourceLocation &location);


void
minute_wrong_length(Lexer &lexer, const SourceLocation &location);


void
second_wrong_length(Lexer &lexer, const SourceLocation &location);


void
month_out_of_range(Lexer &lexer, const SourceLocation &location);


void
day_out_of_range(Lexer &lexer, const SourceLocation &location, s32 days_in_month);


void
hour_out_of_range(Lexer &lexer, const SourceLocation &location);


void
minute_out_of_range(Lexer &lexer, const SourceLocation &location);


void
second_out_of_range(Lexer &lexer, const SourceLocation &location);


void
invalid_expression(Parser &parser, u32 context);


void
invalid_key(Lexer &lexer, const std::string &key);


void
invalid_key(Parser &parser, u32 context);


void
invalid_binary(Lexer &lexer, u32 context);


void
invalid_hexadecimal(Lexer &lexer, u32 context);


void
invalid_octal(Lexer &lexer, u32 context);


void
invalid_value(Lexer &lexer, u32 context);


void
invalid_value(Parser &parser, u32 context);


void
key_redefinition(Parser &parser, const Key &key, const Key &prev);


void
leading_zeros(Lexer &lexer, const SourceLocation &location);


void
missing_array_separator(Parser &parser);


void
missing_inline_table_separator(Parser &parser);


void
missing_keyval(Parser &parser);


void
missing_keyval_separator(Parser &parser);


void
missing_value(Parser &parser);


void
unallowed_sign(Lexer &lexer);


void
unallowed_unicode_codepoint(Lexer &lexer, const SourceLocation &location, u32 codepoint);


//
// TODO Distinguish between unexpected end of file/line and unclosed value?
// TODO Limit errors caused by end of file/line and/or unclosed values?
//

void
unclosed_table_header(Parser &parser, u32 context);


void
unclosed_array(Parser &parser);


void
unclosed_inline_table(Parser& parser);


void
unclosed_string(Lexer &lexer);


}

#endif // TOML_ERROR_HPP
