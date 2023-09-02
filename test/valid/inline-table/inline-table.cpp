#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_inline_table_tests, test_inline_table)
{
    const string toml{"name = { first = \"Tom\", last = \"Preston-Werner\" }\npoint = { x = 1, y = 2 }\nsimple = { a = 1 }\nstr-key = { \"a\" = 1 }\ntable-array = [{ \"a\" = 1 }, { \"b\" = 2 }]\n", 157};

    const Table expected{{ "name", new TableValue({{ "first", new StringValue("Tom") },{ "last", new StringValue("Preston-Werner") },}) },{ "point", new TableValue({{ "x", new IntegerValue(1ULL) },{ "y", new IntegerValue(2ULL) },}) },{ "simple", new TableValue({{ "a", new IntegerValue(1ULL) },}) },{ "str-key", new TableValue({{ "a", new IntegerValue(1ULL) },}) },{ "table-array", new ArrayValue({new TableValue({{ "a", new IntegerValue(1ULL) },}),new TableValue({{ "b", new IntegerValue(2ULL) },}),}) },};

    assert_parsed(toml, expected);
}
