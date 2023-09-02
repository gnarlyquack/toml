#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_inline_table_tests, test_empty)
{
    const string toml{"empty1 = {}\nempty2 = { }\nempty_in_array = [ { not_empty = 1 }, {} ]\nempty_in_array2 = [{},{not_empty=1}]\nmany_empty = [{},{},{}]\nnested_empty = {\"empty\"={}}\n", 157};

    const Table expected{{ "empty1", new TableValue({}) },{ "empty2", new TableValue({}) },{ "empty_in_array", new ArrayValue({new TableValue({{ "not_empty", new IntegerValue(1ULL) },}),new TableValue({}),}) },{ "empty_in_array2", new ArrayValue({new TableValue({}),new TableValue({{ "not_empty", new IntegerValue(1ULL) },}),}) },{ "many_empty", new ArrayValue({new TableValue({}),new TableValue({}),new TableValue({}),}) },{ "nested_empty", new TableValue({{ "empty", new TableValue({}) },}) },};

    assert_parsed(toml, expected);
}
