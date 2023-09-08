//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#include "test_common.hpp"

#include <gtest/gtest.h>


using namespace std;
using namespace toml;



TEST(parse, comments)
{
    const string toml =
        "# This is a full-line comment\n"
        "key = \"value\"  # This is a comment at the end of a line\n"
        "another = \"# This is not a comment\"\n"
        ;

    const Table expected = {
        { "key", Value::of_string("value") },
        { "another", Value::of_string("# This is not a comment") },
    };

    assert_parsed(toml, expected);
}


TEST(parse, keyvals)
{
    const string toml = "key = \"value\"";

    const Table result = {
        { "key", Value::of_string("value") },
    };

    assert_parsed(toml, result);
}


TEST(parse, invalid_keyvals)
{
    const string toml =
        "key = # INVALID\n"
        "first = \"Tom\" last = \"Preston-Werner\" # INVALID\n"
        ;

    const vector<Error> errors = {
        { 1, 7, "Missing value." },
        { 2, 15, "Expected the end of the line but got: last = \"Preston-Werner\"" },
    };

    assert_errors(toml, errors);
}


TEST(parse, bare_keys)
{
    const string toml =
        "key = \"value\"\n"
        "bare_key = \"value\"\n"
        "bare-key = \"value\"\n"
        "1234 = \"value\"\n"
        ;

    const Table result = {
        { "key", Value::of_string("value") },
        { "bare_key", Value::of_string("value") },
        { "bare-key", Value::of_string("value") },
        { "1234", Value::of_string("value") },
    };

    assert_parsed(toml, result);
}


TEST(parse, quoted_keys)
{
    const string toml =
        "\"127.0.0.1\" = \"value\"\n"
        "\"character encoding\" = \"value\"\n"
        "\"ʎǝʞ\" = \"value\"\n"
        "'key2' = \"value\"\n"
        "'quoted \"value\"' = \"value\"\n"
        ;

    const Table result = {
        { "127.0.0.1", Value::of_string("value") },
        { "character encoding", Value::of_string("value") },
        { "ʎǝʞ", Value::of_string("value") },
        { "key2", Value::of_string("value") },
        { "quoted \"value\"", Value::of_string("value") },
    };

    assert_parsed(toml, result);
}


TEST(parse, empty_unqoted_key)
{
    const string toml = "= \"no key name\"  # INVALID";

    const vector<Error> errors = {
        { 1, 1, "Missing key." },
    };

    assert_errors(toml, errors);
}


TEST(parse, empty_string_key)
{
    const string toml = "\"\" = \"blank\"     # VALID but discouraged";

    const Table result = {
        { "", Value::of_string("blank") } ,
    };

    assert_parsed(toml, result);
}


TEST(parse, empty_literal_key)
{
    const string toml = "'' = 'blank'     # VALID but discouraged";

    const Table result = {
        { "", Value::of_string("blank") } ,
    };

    assert_parsed(toml, result);
}


TEST(parse, dotted_keys)
{
    const string toml =
        "name = \"Orange\"\n"
        "physical.color = \"orange\"\n"
        "physical.shape = \"round\"\n"
        "site.\"google.com\" = true\n"
        "fruit.name = \"banana\"     # this is best practice\n"
        "fruit. color = \"yellow\"    # same as fruit.color\n"
        "fruit . flavor = \"banana\"   # same as fruit.flavor\n"
        "3.14159 = \"pi\"\n"
        ;

    const Table result = {
        { "name", Value::of_string("Orange") },
        { "physical", Value::of_table({
                    { "color", Value::of_string("orange") },
                    { "shape", Value::of_string("round") } }) },
        { "site", Value::of_table({
                    { "google.com", Value::of_boolean(true) } }) },
        { "fruit", Value::of_table({
                    { "name", Value::of_string("banana") },
                    { "color", Value::of_string("yellow") },
                    { "flavor", Value::of_string("banana") } }) },
        { "3", Value::of_table({
                    { "14159", Value::of_string("pi") } }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, multiple_keys)
{
    const string toml =
        "# DO NOT DO THIS\n"
        "name = \"Tom\"\n"
        "name = \"Pradyun\"\n"
        "# THIS WILL NOT WORK\n"
        "spelling = \"favorite\"\n"
        "\"spelling\" = \"favourite\"\n"
        ;

    const vector<Error> errors = {
        { 3, 1, "Key 'name' has already been defined on line 2, character 1." },
        { 6, 1, "Key 'spelling' has already been defined on line 5, character 1." },
    };

    assert_errors(toml, errors);
}


TEST(parse, extend_implicit_key)
{
    const string toml =
        "# This makes the key \"fruit\" into a table.\n"
        "fruit.apple.smooth = true\n"
        "\n"
        "# So then you can add to the table \"fruit\" like so:\n"
        "fruit.orange = 2\n"
        "\n"
        "fruit.apple.color = \"red\"\n"
        "# Defines a table named fruit\n"
        "# Defines a table named fruit.apple\n"
        "\n"
        "fruit.apple.taste.sweet = true\n"
        "# Defines a table named fruit.apple.taste\n"
        "# fruit and fruit.apple were already created\n"
        ;

    const Table result = {
        { "fruit", Value::of_table({
                    { "apple", Value::of_table({
                                { "smooth", Value::of_boolean(true) },
                                { "color", Value::of_string("red") },
                                { "taste", Value::of_table({
                                        { "sweet", Value::of_boolean(true) },
                                    }) },
                            }) },
                    { "orange", Value::of_integer(2) } }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, key_redefinition)
{
    const string toml =
        "# THE FOLLOWING IS INVALID\n"
        "\n"
        "# This defines the value of fruit.apple to be an integer.\n"
        "fruit.apple = 1\n"
        "\n"
        "# But then this treats fruit.apple like it's a table.\n"
        "# You can't turn an integer into a table.\n"
        "fruit.apple.smooth = true\n"
        ;

    const vector<Error> errors = {
        { 8, 7, "Key 'apple' has already been defined on line 4, character 7." },
    };

    assert_errors(toml, errors);
}


TEST(parse, out_of_order_keys)
{
    const string toml =
        "# VALID BUT DISCOURAGED\n"
        "\n"
        "apple.type = \"fruit\"\n"
        "orange.type = \"fruit\"\n"
        "\n"
        "apple.skin = \"thin\"\n"
        "orange.skin = \"thick\"\n"
        "\n"
        "apple.color = \"red\"\n"
        "orange.color = \"orange\"\n"
        ;

    const Table result = {
        { "apple", Value::of_table({
                { "type", Value::of_string("fruit") },
                { "skin", Value::of_string("thin") },
                { "color", Value::of_string("red") } }) },
        { "orange", Value::of_table({
                { "type", Value::of_string("fruit") },
                { "skin", Value::of_string("thick") },
                { "color", Value::of_string("orange") } }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, basic_strings)
{
    const string toml =
        "str = \"I'm a string. \\\"You can quote me\\\". Name\\tJos\\u00e9\\nLocation\\tSF.\"\n"
        ;

    const Table result = {
        { "str", Value::of_string("I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF.") },
    };

    assert_parsed(toml, result);
}


TEST(parse, multiline_basic_strings)
{
    const string toml =
        "str1 = \"\"\"\n"
        "Roses are red\n"
        "Violets are blue\"\"\"\n"
        ;

    const Table result = {
        { "str1", Value::of_string("Roses are red\nViolets are blue") },
    };

    assert_parsed(toml, result);
}


TEST(parse, line_ending_backslash)
{
    const string toml =
        "# The following strings are byte-for-byte equivalent:\n"
        "str1 = \"The quick brown fox jumps over the lazy dog.\"\n"
        "\n"
        "str2 = \"\"\"\n"
        "The quick brown \\\n"
        "\n"
        "\n"
        "  fox jumps over \\\n"
        "    the lazy dog.\"\"\"\n"
        "\n"
        "str3 = \"\"\"\\\n"
        "       The quick brown \\\n"
        "       fox jumps over \\\n"
        "       the lazy dog.\\\n"
        "       \"\"\"\n"
        ;

    const Table result = {
        { "str1", Value::of_string("The quick brown fox jumps over the lazy dog.") },
        { "str2", Value::of_string("The quick brown fox jumps over the lazy dog.") },
        { "str3", Value::of_string("The quick brown fox jumps over the lazy dog.") },
    };

    assert_parsed(toml, result);
}


TEST(parse, multiline_basic_string_escapes)
{
    const string toml =
        "str4 = \"\"\"Here are two quotation marks: \"\". Simple enough.\"\"\"\n"
        "# str5 = \"\"\"Here are three quotation marks: \"\"\".\"\"\"  # INVALID\n"
        "str5 = \"\"\"Here are three quotation marks: \"\"\\\".\"\"\"\n"
        "str6 = \"\"\"Here are fifteen quotation marks: \"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\".\"\"\"\n"
        "\n"
        "# \"This,\" she said, \"is just a pointless statement.\"\n"
        "str7 = \"\"\"\"This,\" she said, \"is just a pointless statement.\"\"\"\"\n"
        ;

    const Table result = {
        { "str4", Value::of_string("Here are two quotation marks: \"\". Simple enough.") },
        { "str5", Value::of_string("Here are three quotation marks: \"\"\".") },
        { "str6", Value::of_string("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\".") },
        { "str7", Value::of_string("\"This,\" she said, \"is just a pointless statement.\"") },
    };

    assert_parsed(toml, result);
}


TEST(parse, literal_strings)
{
    const string toml =
        "# What you see is what you get.\n"
        "winpath  = 'C:\\Users\\nodejs\\templates'\n"
        "winpath2 = '\\\\ServerX\\admin$\\system32\\'\n"
        "quoted   = 'Tom \"Dubs\" Preston-Werner'\n"
        "regex    = '<\\i\\c*\\s*>'\n"
        ;

    const Table result = {
        { "winpath", Value::of_string("C:\\Users\\nodejs\\templates") },
        { "winpath2", Value::of_string("\\\\ServerX\\admin$\\system32\\") },
        { "quoted", Value::of_string("Tom \"Dubs\" Preston-Werner") },
        { "regex", Value::of_string("<\\i\\c*\\s*>") },
    };

    assert_parsed(toml, result);
}


TEST(parse, multiline_literal_strings)
{
    const string toml =
        "regex2 = '''I [dw]on't need \\d{2} apples'''\n"
        "lines  = '''\n"
        "The first newline is\n"
        "trimmed in raw strings.\n"
        "   All other whitespace\n"
        "   is preserved.\n"
        "'''\n"
        ;

    const Table result = {
        { "regex2", Value::of_string("I [dw]on't need \\d{2} apples") },
        { "lines", Value::of_string("The first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n") },
    };

    assert_parsed(toml, result);
}


TEST(parse, mulitiline_literal_string_escapes)
{
    const string toml =
        "quot15 = '''Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"'''\n"
        "\n"
        "# apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID\n"
        "apos15 = \"Here are fifteen apostrophes: '''''''''''''''\"\n"
        "\n"
        "# 'That,' she said, 'is still pointless.'\n"
        "str = ''''That,' she said, 'is still pointless.''''\n"
        ;

    const Table result = {
        { "quot15", Value::of_string("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"") },
        { "apos15", Value::of_string("Here are fifteen apostrophes: '''''''''''''''") },
        { "str", Value::of_string("'That,' she said, 'is still pointless.'") },
    };

    assert_parsed(toml, result);
}


TEST(parse, integers)
{
    const string toml =
        "int1 = +99\n"
        "int2 = 42\n"
        "int3 = 0\n"
        "int4 = -17\n"
        "int5 = 1_000\n"
        "int6 = 5_349_221\n"
        "int7 = 53_49_221  # Indian number system grouping\n"
        "int8 = 1_2_3_4_5  # VALID but discouraged\n"
        "\n"
        "# hexadecimal with prefix `0x`\n"
        "hex1 = 0xDEADBEEF\n"
        "hex2 = 0xdeadbeef\n"
        "hex3 = 0xdead_beef\n"
        "\n"
        "# octal with prefix `0o`\n"
        "oct1 = 0o01234567\n"
        "oct2 = 0o755 # useful for Unix file permissions\n"
        "\n"
        "# binary with prefix `0b`\n"
        "bin1 = 0b11010110\n"
        ;

    const Table result = {
        { "int1", Value::of_integer(99) },
        { "int2", Value::of_integer(42) },
        { "int3", Value::of_integer(0) },
        { "int4", Value::of_integer(-17) },
        { "int5", Value::of_integer(1000) },
        { "int6", Value::of_integer(5349221) },
        { "int7", Value::of_integer(5349221) },
        { "int8", Value::of_integer(12345) },

        { "hex1", Value::of_integer(0xDEADBEEF) },
        { "hex2", Value::of_integer(0xdeadbeef) },
        { "hex3", Value::of_integer(0xdeadbeef) },

        { "oct1", Value::of_integer(01234567) },
        { "oct2", Value::of_integer(0755) },

        { "bin1", Value::of_integer(214) },
    };

    assert_parsed(toml, result);
}


TEST(parse, floats)
{
    const string toml =
        "# fractional\n"
        "flt1 = +1.0\n"
        "flt2 = 3.1415\n"
        "flt3 = -0.01\n"
        "\n"
        "# exponent\n"
        "flt4 = 5e+22\n"
        "flt5 = 1e06\n"
        "flt6 = -2E-2\n"
        "\n"
        "# both\n"
        "flt7 = 6.626e-34\n"
        "\n"
        "flt8 = 224_617.445_991_228\n"
        ;

    const Table result = {
        { "flt1", Value::of_float(1) },
        { "flt2", Value::of_float(3.1415) },
        { "flt3", Value::of_float(-0.01) },
        { "flt4", Value::of_float(5e+22) },
        { "flt5", Value::of_float(1e06) },
        { "flt6", Value::of_float(-2e-2) },
        { "flt7", Value::of_float(6.626e-34) },
        { "flt8", Value::of_float(224617.445991228) },
    };

    assert_parsed(toml, result);
}


TEST(parse, invalid_floats)
{
    const string toml =
        "# INVALID FLOATS\n"
        "invalid_float_1 = .7\n"
        "invalid_float_2 = 7.\n"
        "invalid_float_3 = 3.e+20\n"
        ;

    const vector<Error> errors = {
        { 2, 19, "Invalid decimal value: .7" },
        { 3, 21, "Missing fractional part of decimal number." },
        { 4, 21, "Missing fractional part of decimal number." },
    };

    assert_errors(toml, errors);
}


TEST(parse, infinity)
{
    const string toml =
        "# infinity\n"
        "sf1 = inf  # positive infinity\n"
        "sf2 = +inf # positive infinity\n"
        "sf3 = -inf # negative infinity\n"
        ;

    const Table result = {
        { "sf1", Value::of_float(INF64) },
        { "sf2", Value::of_float(+INF64) },
        { "sf3", Value::of_float(-INF64) },
    };

    assert_parsed(toml, result);
}


TEST(parse, nan)
{
    const string toml =
        "# not a number\n"
        "sf4 = nan  # actual sNaN/qNaN encoding is implementation-specific\n"
        "sf5 = +nan # same as `nan`\n"
        "sf6 = -nan # valid, actual encoding is implementation-specific\n"
        ;

    const Table result = {
        { "sf4", Value::of_float(NAN64) },
        { "sf5", Value::of_float(+NAN64) },
        { "sf6", Value::of_float(-NAN64) },
    };

    assert_parsed(toml, result);
}


TEST(parse, booleans)
{
    const string toml =
        "bool1 = true\n"
        "bool2 = false\n"
        ;

    const Table result = {
        { "bool1", Value::of_boolean(true) },
        { "bool2", Value::of_boolean(false) },
    };

    assert_parsed(toml, result);
}


TEST(parse, local_times)
{
    const string toml{
        "lt1 = 07:32:00\n"
        "lt2 = 00:32:00.999999\n"
    };

    const Table result{
        { "lt1", Value::of_local_time(LocalTime(chrono::hours(7) + chrono::minutes(32))) },
        { "lt2", Value::of_local_time(LocalTime(chrono::minutes(32) + chrono::microseconds(999999))) },
    };

    assert_parsed(toml, result);
}


TEST(parse, local_dates)
{
    const string toml{"ld1 = 1979-05-27"};

    const Table result{
        { "ld1", Value::of_local_date(LocalDate(date::year(1979) / date::month(5) / date::day(27))) },
    };

    assert_parsed(toml, result);
}


TEST(parse, local_datetimes)
{
    const string toml{
        "ldt1 = 1979-05-27T07:32:00\n"
        "ldt2 = 1979-05-27T00:32:00.999999\n"
    };

    const Table result{
        { "ldt1",
            Value::of_local_datetime(
                date::local_time<date::days>(date::year(1979) / date::month(5) / date::day(27))
                + chrono::hours(7) + chrono::minutes(32)) },
        { "ldt2",
            Value::of_local_datetime(
                date::local_time<date::days>(date::year(1979) / date::month(5) / date::day(27))
                + chrono::minutes(32) + chrono::microseconds(999999)) },
    };

    assert_parsed(toml, result);
}


TEST(parse, offset_datetimes)
{
    const string toml{
        "odt1 = 1979-05-27T07:32:00Z\n"
        "odt2 = 1979-05-27T00:32:00-07:00\n"
        "odt3 = 1979-05-27T00:32:00.999999-07:00\n"
        "odt4 = 1979-05-27 07:32:00Z\n"
    };

    const Table result{
        { "odt1",
            Value::of_offset_datetime(
                date::sys_days(date::year(1979) / date::month(5) / date::day(27))
                + chrono::hours(7) + chrono::minutes(32)) },

        { "odt2",
            Value::of_offset_datetime(
                date::sys_days(date::year(1979) / date::month(5) / date::day(27))
                + chrono::hours(7) + chrono::minutes(32)) },

        { "odt3",
            Value::of_offset_datetime(
                date::sys_days(date::year(1979) / date::month(5) / date::day(27))
                + chrono::hours(7) + chrono::minutes(32) + chrono::microseconds(999999)) },

        { "odt4",
            Value::of_offset_datetime(
                date::sys_days(date::year(1979) / date::month(5) / date::day(27))
                + chrono::hours(7) + chrono::minutes(32)) },
    };

    assert_parsed(toml, result);
}


TEST(parse, arrays)
{
    const string toml =
        "integers = [ 1, 2, 3 ]\n"
        "colors = [ \"red\", \"yellow\", \"green\" ]\n"
        "nested_arrays_of_ints = [ [ 1, 2 ], [3, 4, 5] ]\n"
        "nested_mixed_array = [ [ 1, 2 ], [\"a\", \"b\", \"c\"] ]\n"
        "string_array = [ \"all\", 'strings', \"\"\"are the same\"\"\", '''type''' ]\n"
        "\n"
        "# Mixed-type arrays are allowed\n"
        "numbers = [ 0.1, 0.2, 0.5, 1, 2, 5 ]\n"
        "contributors = [\n"
        "  \"Foo Bar <foo@example.com>\",\n"
        "  { name = \"Baz Qux\", email = \"bazqux@example.com\", url = \"https://example.com/bazqux\" }\n"
        "]\n"
        ;

    const Table result = {
        { "integers", Value::of_array({Value::of_integer(1), Value::of_integer(2), Value::of_integer(3) }) },
        { "colors", Value::of_array({Value::of_string("red"), Value::of_string("yellow"), Value::of_string("green") }) },
        { "nested_arrays_of_ints", Value::of_array({
                Value::of_array({ Value::of_integer(1), Value::of_integer(2) }),
                Value::of_array({ Value::of_integer(3), Value::of_integer(4), Value::of_integer(5) }),
            }) },
        { "nested_mixed_array", Value::of_array({
                Value::of_array({ Value::of_integer(1), Value::of_integer(2) }),
                Value::of_array({ Value::of_string("a"), Value::of_string("b"), Value::of_string("c") }),
            }) },
        { "string_array", Value::of_array({
                Value::of_string("all"), Value::of_string("strings"), Value::of_string("are the same"), Value::of_string("type"),
            }) },
        { "numbers", Value::of_array({
                Value::of_float(0.1), Value::of_float(0.2), Value::of_float(0.5), Value::of_integer(1), Value::of_integer(2), Value::of_integer(5),
            }) },
        { "contributors", Value::of_array({
                Value::of_string("Foo Bar <foo@example.com>"),
                Value::of_table({
                    { "name", Value::of_string("Baz Qux") },
                    { "email", Value::of_string("bazqux@example.com") },
                    { "url", Value::of_string("https://example.com/bazqux") },
                }),
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, multiline_arrays)
{
    const string toml =
        "integers2 = [\n"
        "  1, 2, 3\n"
        "]\n"
        "\n"
        "integers3 = [\n"
        "  1,\n"
        "  2, # this is ok\n"
        "]\n"
        ;

    const Table result = {
        { "integers2", Value::of_array({
                Value::of_integer(1), Value::of_integer(2), Value::of_integer(3),
            }) },
        { "integers3", Value::of_array({
                Value::of_integer(1), Value::of_integer(2),
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, inline_tables)
{
    const string toml =
        "name = { first = \"Tom\", last = \"Preston-Werner\" }\n"
        "point = { x = 1, y = 2 }\n"
        "animal = { type.name = \"pug\" }\n"
        ;

    const Table result = {
        { "name", Value::of_table({
                { "first", Value::of_string("Tom") },
                { "last", Value::of_string("Preston-Werner") },
            }) },
        { "point", Value::of_table({
                { "x", Value::of_integer(1) },
                { "y", Value::of_integer(2) },
            }) },
        { "animal", Value::of_table({
                { "type", Value::of_table({ { "name", Value::of_string("pug") } }) },
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, tables)
{
    const string toml =
        "[table]\n"
        "\n"
        "[table-1]\n"
        "key1 = \"some string\"\n"
        "key2 = 123\n"
        "\n"
        "[table-2]\n"
        "key1 = \"another string\"\n"
        "key2 = 456\n"
        "\n"
        "[dog.\"tater.man\"]\n"
        "type.name = \"pug\"\n"
        ;

    const Table result = {
        { "table", Value::of_table() },
        { "table-1", Value::of_table({
                { "key1", Value::of_string("some string") },
                { "key2", Value::of_integer(123) },
            }) },
        { "table-2", Value::of_table({
                { "key1", Value::of_string("another string") },
                { "key2", Value::of_integer(456) },
            }) },
        { "dog", Value::of_table({
                { "tater.man", Value::of_table({
                        { "type", Value::of_table({
                                { "name", Value::of_string("pug") },
                            }) },
                    }) },
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, spaces_in_table_headers)
{
    const string toml =
        "[a.b.c]            # this is best practice\n"
        "[ d.e.f ]          # same as [d.e.f]\n"
        "[ g .  h  . i ]    # same as [g.h.i]\n"
        "[ j . \"ʞ\" . 'l' ]  # same as [j.\"ʞ\".'l']\n"
        ;

    const Table result = {
        { "a", Value::of_table({
                { "b", Value::of_table({
                        { "c", Value::of_table() },
                    }) },
            }) },
        { "d", Value::of_table({
                { "e", Value::of_table({
                        { "f", Value::of_table() },
                    }) },
            }) },
        { "g", Value::of_table({
                { "h", Value::of_table({
                        { "i", Value::of_table() },
                    }) },
            }) },
        { "j", Value::of_table({
                { "ʞ", Value::of_table({
                        { "l", Value::of_table() },
                    }) },
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, implicit_super_tables)
{
    const string toml =
        "# [x] you\n"
        "# [x.y] don't\n"
        "# [x.y.z] need these\n"
        "[x.y.z.w] # for this to work\n"
        "\n"
        "[x] # defining a super-table afterward is ok\n"
        ;

    const Table result = {
        { "x", Value::of_table({
                { "y", Value::of_table({
                        { "z", Value::of_table({
                                { "w", Value::of_table() },
                            }) },
                    }) },
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, cannot_redefine_table)
{
    const string toml =
        "# DO NOT DO THIS\n"
        "\n"
        "[fruit]\n"
        "apple = \"red\"\n"
        "\n"
        "[fruit]\n"
        "orange = \"orange\"\n"
        ;

    const vector<Error> errors = {
        { 6, 2, "Key 'fruit' has already been defined on line 3, character 2." },
    };

    assert_errors(toml, errors);
}


TEST(parse, cannot_redefine_subtable)
{
    const string toml =
        "# DO NOT DO THIS EITHER\n"
        "\n"
        "[fruit]\n"
        "apple = \"red\"\n"
        "\n"
        "[fruit.apple]\n"
        "texture = \"smooth\"\n"
        ;

    const vector<Error> errors = {
        { 6, 8, "Key 'apple' has already been defined on line 4, character 1." },
    };

    assert_errors(toml, errors);
}


TEST(parse, out_of_order_table_definitions)
{
    const string toml =
        "# VALID BUT DISCOURAGED\n"
        "[fruit.apple]\n"
        "[animal]\n"
        "[fruit.orange]\n"
        ;

    const Table result = {
        { "fruit", Value::of_table({
                { "apple", Value::of_table() },
                { "orange", Value::of_table() },
            }) },
        { "animal", Value::of_table() },
    };

    assert_parsed(toml, result);
}


TEST(parse, top_level_table)
{
    const string toml =
        "# Top-level table begins.\n"
        "name = \"Fido\"\n"
        "breed = \"pug\"\n"
        "\n"
        "# Top-level table ends.\n"
        "[owner]\n"
        "name = \"Regina Dogman\"\n"
        "member_since = 1999-08-04\n"
        ;

    const Table result = {
        { "name", Value::of_string("Fido") },
        { "breed", Value::of_string("pug") },
        { "owner", Value::of_table({
                { "name", Value::of_string("Regina Dogman") },
                { "member_since", Value::of_local_date(LocalDate(date::year(1999) / date::month(8) / date::day(4))) },
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, dotted_keys_cannot_redefine_tables)
{
    const string toml =
        "[fruit]\n"
        "apple.color = \"red\"\n"
        "apple.taste.sweet = true\n"
        "\n"
        "[fruit.apple]  # INVALID\n"
        ;

    const vector<Error> errors = {
        { 5, 8, "Key 'apple' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, errors);
}


TEST(parse, dotted_keys_cannot_redefine_subtables)
{
    const string toml =
        "[fruit]\n"
        "apple.color = \"red\"\n"
        "apple.taste.sweet = true\n"
        "\n"
        "[fruit.apple.taste]  # INVALID\n"
        ;

    const vector<Error> errors = {
        { 5, 14, "Key 'taste' has already been defined on line 3, character 7." },
    };

    assert_errors(toml, errors);
}


TEST(parse, dotted_keys_can_add_subtables)
{
    const string toml =
        "[fruit]\n"
        "apple.color = \"red\"\n"
        "apple.taste.sweet = true\n"
        "\n"
        "[fruit.apple.texture]  # you can add sub-tables\n"
        "smooth = true\n"
        ;

    const Table expected{
        { "fruit", Value::of_table({
                { "apple", Value::of_table({
                        { "color", Value::of_string("red") },
                        { "taste", Value::of_table({
                                { "sweet", Value::of_boolean(true) },
                            }) },
                        { "texture", Value::of_table({
                                { "smooth", Value::of_boolean(true) },
                            }) },
                    }) },
            }) },
    };

    assert_parsed(toml, expected);
}


TEST(parse, inline_tables_cannot_be_extended)
{
    const string toml =
        "[product]\n"
        "type = { name = \"Nail\" }\n"
        "type.edible = false  # INVALID\n"
        ;

    const vector<Error> errors = {
        { 3, 1, "Key 'type' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, errors);
}


TEST(parse, inline_tables_cannot_extend_other_tables)
{
    const string toml =
        "[product]\n"
        "type.name = \"Nail\"\n"
        "type = { edible = false }  # INVALID\n"
        ;

    const vector<Error> errors = {
        { 3, 1, "Key 'type' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, errors);
}


TEST(parse, table_arrays)
{
    const string toml =
        "[[products]]\n"
        "name = \"Hammer\"\n"
        "sku = 738594937\n"
        "\n"
        "[[products]]  # empty table within the array\n"
        "\n"
        "[[products]]\n"
        "name = \"Nail\"\n"
        "sku = 284758393\n"
        "\n"
        "color = \"gray\"\n"
        ;

    const Table result = {
        { "products", Value::of_array({
                Value::of_table({
                        { "name", Value::of_string("Hammer") },
                        { "sku", Value::of_integer(738594937) },
                    }),
                Value::of_table(),
                Value::of_table({
                        { "name", Value::of_string("Nail") },
                        { "sku", Value::of_integer(284758393) },
                        { "color", Value::of_string("gray") }
                    }),
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, table_array_sub_elements)
{
    const string toml =
        "[[fruits]]\n"
        "name = \"apple\"\n"
        "\n"
        "[fruits.physical]  # subtable\n"
        "color = \"red\"\n"
        "shape = \"round\"\n"
        "\n"
        "[[fruits.varieties]]  # nested array of tables\n"
        "name = \"red delicious\"\n"
        "\n"
        "[[fruits.varieties]]\n"
        "name = \"granny smith\"\n"
        "\n"
        "\n"
        "[[fruits]]\n"
        "name = \"banana\"\n"
        "\n"
        "[[fruits.varieties]]\n"
        "name = \"plantain\"\n"
        ;

    const Table result = {
        { "fruits", Value::of_array({
                Value::of_table({
                        { "name", Value::of_string("apple") },
                        { "physical", Value::of_table({
                                { "color", Value::of_string("red") },
                                { "shape", Value::of_string("round") },
                            }) },
                        { "varieties", Value::of_array({
                                Value::of_table({
                                    { "name", Value::of_string("red delicious") },
                                }),
                                Value::of_table({
                                    { "name", Value::of_string("granny smith") },
                                }),
                            }) },
                    }),
                Value::of_table({
                        { "name", Value::of_string("banana") },
                        { "varieties", Value::of_array({
                                Value::of_table({
                                        { "name", Value::of_string("plantain") },
                                    }),
                            }) },
                    }),
            }) },
    };

    assert_parsed(toml, result);
}


TEST(parse, cannot_redefine_table_as_table_array)
{
    const string toml =
        "# INVALID TOML DOC\n"
        "[fruit.physical]  # subtable, but to which parent element should it belong?\n"
        "color = \"red\"\n"
        "shape = \"round\"\n"
        "\n"
        "[[fruit]]  # parser must throw an error upon discovering that \"fruit\" is\n"
        "           # an array rather than a table\n"
        "name = \"apple\"\n"
        ;

    const vector<Error> errors = {
        { 6, 3, "Key 'fruit' has already been defined on line 2, character 2." },
    };

    assert_errors(toml, errors);
}


TEST(parse, table_array_cannot_extend_array)
{
    const string toml =
        "# INVALID TOML DOC\n"
        "fruits = []\n"
        "\n"
        "[[fruits]] # Not allowed\n"
        ;

    const vector<Error> errors = {
        { 4, 3, "Key 'fruits' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, errors);
}


TEST(parse, tables_and_table_arrays_cannot_redefine_each_other)
{
    const string toml =
        "# INVALID TOML DOC\n"
        "[[fruits]]\n"
        "name = \"apple\"\n"
        "\n"
        "[[fruits.varieties]]\n"
        "name = \"red delicious\"\n"
        "\n"
        "# INVALID: This table conflicts with the previous array of tables\n"
        "[fruits.varieties]\n"
        "name = \"granny smith\"\n"
        "\n"
        "[fruits.physical]\n"
        "color = \"red\"\n"
        "shape = \"round\"\n"
        "\n"
        "# INVALID: This array of tables conflicts with the previous table\n"
        "[[fruits.physical]]\n"
        "color = \"green\"\n"
        ;

    const vector<Error> errors = {
        { 9, 9, "Key 'varieties' has already been defined on line 5, character 10." },
        { 17, 10, "Key 'physical' has already been defined on line 12, character 9." },
    };

    assert_errors(toml, errors);
}
