#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_inline_table_tests, test_empty)
{
    const string toml = "empty1 = {}\nempty2 = { }\nempty_in_array = [ { not_empty = 1 }, {} ]\nempty_in_array2 = [{},{not_empty=1}]\nmany_empty = [{},{},{}]\nnested_empty = {\"empty\"={}}\n";

    const Table expected{{ "empty1", new TableValue({}) },{ "empty2", new TableValue({}) },{ "empty_in_array", new ArrayValue({new TableValue({{ "not_empty", new IntegerValue(1ULL) },}),new TableValue({}),}) },{ "empty_in_array2", new ArrayValue({new TableValue({}),new TableValue({{ "not_empty", new IntegerValue(1ULL) },}),}) },{ "many_empty", new ArrayValue({new TableValue({}),new TableValue({}),new TableValue({}),}) },{ "nested_empty", new TableValue({{ "empty", new TableValue({}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_inline_table_tests, test_nest)
{
    const string toml = "tbl_tbl_empty = { tbl_0 = {} }\ntbl_tbl_val   = { tbl_1 = { one = 1 } }\ntbl_arr_tbl   = { arr_tbl = [ { one = 1 } ] }\narr_tbl_tbl   = [ { tbl = { one = 1 } } ]\n\n# Array-of-array-of-table is interesting because it can only\n# be represented in inline form.\narr_arr_tbl_empty = [ [ {} ] ]\narr_arr_tbl_val = [ [ { one = 1 } ] ]\narr_arr_tbls  = [ [ { one = 1 }, { two = 2 } ] ]\n";

    const Table expected{{ "arr_arr_tbl_empty", new ArrayValue({new ArrayValue({new TableValue({}),}),}) },{ "arr_arr_tbl_val", new ArrayValue({new ArrayValue({new TableValue({{ "one", new IntegerValue(1ULL) },}),}),}) },{ "arr_arr_tbls", new ArrayValue({new ArrayValue({new TableValue({{ "one", new IntegerValue(1ULL) },}),new TableValue({{ "two", new IntegerValue(2ULL) },}),}),}) },{ "arr_tbl_tbl", new ArrayValue({new TableValue({{ "tbl", new TableValue({{ "one", new IntegerValue(1ULL) },}) },}),}) },{ "tbl_arr_tbl", new TableValue({{ "arr_tbl", new ArrayValue({new TableValue({{ "one", new IntegerValue(1ULL) },}),}) },}) },{ "tbl_tbl_empty", new TableValue({{ "tbl_0", new TableValue({}) },}) },{ "tbl_tbl_val", new TableValue({{ "tbl_1", new TableValue({{ "one", new IntegerValue(1ULL) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_inline_table_tests, test_inline_table)
{
    const string toml = "name = { first = \"Tom\", last = \"Preston-Werner\" }\npoint = { x = 1, y = 2 }\nsimple = { a = 1 }\nstr-key = { \"a\" = 1 }\ntable-array = [{ \"a\" = 1 }, { \"b\" = 2 }]\n";

    const Table expected{{ "name", new TableValue({{ "first", new StringValue("Tom") },{ "last", new StringValue("Preston-Werner") },}) },{ "point", new TableValue({{ "x", new IntegerValue(1ULL) },{ "y", new IntegerValue(2ULL) },}) },{ "simple", new TableValue({{ "a", new IntegerValue(1ULL) },}) },{ "str-key", new TableValue({{ "a", new IntegerValue(1ULL) },}) },{ "table-array", new ArrayValue({new TableValue({{ "a", new IntegerValue(1ULL) },}),new TableValue({{ "b", new IntegerValue(2ULL) },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_inline_table_tests, test_key_dotted)
{
    const string toml = "inline = {a.b = 42}\n\nmany.dots.here.dot.dot.dot = {a.b.c = 1, a.b.d = 2}\n\na = {   a.b  =  1   }\nb = {   \"a\".\"b\"  =  1   }\nc = {   a   .   b  =  1   }\nd = {   'a'   .   \"b\"  =  1   }\ne = {a.b=1}\n\n[tbl]\na.b.c = {d.e=1}\n\n[tbl.x]\na.b.c = {d.e=1}\n\n[[arr]]\nt = {a.b=1}\nT = {a.b=1}\n\n[[arr]]\nt = {a.b=2}\nT = {a.b=2}\n";

    const Table expected{{ "a", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "arr", new ArrayValue({new TableValue({{ "T", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "t", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },}),new TableValue({{ "T", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(2ULL) },}) },}) },{ "t", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(2ULL) },}) },}) },}),}) },{ "b", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "c", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "d", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "e", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "inline", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(42ULL) },}) },}) },{ "many", new TableValue({{ "dots", new TableValue({{ "here", new TableValue({{ "dot", new TableValue({{ "dot", new TableValue({{ "dot", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new IntegerValue(1ULL) },{ "d", new IntegerValue(2ULL) },}) },}) },}) },}) },}) },}) },}) },}) },{ "tbl", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "d", new TableValue({{ "e", new IntegerValue(1ULL) },}) },}) },}) },}) },{ "x", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "d", new TableValue({{ "e", new IntegerValue(1ULL) },}) },}) },}) },}) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_inline_table_tests, test_multiline)
{
    const string toml = "tbl_multiline = { a = 1, b = \"\"\"\nmultiline\n\"\"\", c = \"\"\"and yet\nanother line\"\"\", d = 4 }\n";

    const Table expected{{ "tbl_multiline", new TableValue({{ "a", new IntegerValue(1ULL) },{ "b", new StringValue("multiline\n") },{ "c", new StringValue("and yet\nanother line") },{ "d", new IntegerValue(4ULL) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_inline_table_tests, test_array)
{
    const string toml = "people = [{first_name = \"Bruce\", last_name = \"Springsteen\"},\n          {first_name = \"Eric\", last_name = \"Clapton\"},\n          {first_name = \"Bob\", last_name = \"Seger\"}]\n";

    const Table expected{{ "people", new ArrayValue({new TableValue({{ "first_name", new StringValue("Bruce") },{ "last_name", new StringValue("Springsteen") },}),new TableValue({{ "first_name", new StringValue("Eric") },{ "last_name", new StringValue("Clapton") },}),new TableValue({{ "first_name", new StringValue("Bob") },{ "last_name", new StringValue("Seger") },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_inline_table_tests, test_end_in_bool)
{
    const string toml = "black = { python=\">3.6\", version=\">=18.9b0\", allow_prereleases=true }\n";

    const Table expected{{ "black", new TableValue({{ "allow_prereleases", new BooleanValue(true) },{ "python", new StringValue(">3.6") },{ "version", new StringValue(">=18.9b0") },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_inline_table_tests, test_bool)
{
    const string toml = "a = {a = true, b = false}\n";

    const Table expected{{ "a", new TableValue({{ "a", new BooleanValue(true) },{ "b", new BooleanValue(false) },}) },};

    assert_parsed(toml, expected);
}
