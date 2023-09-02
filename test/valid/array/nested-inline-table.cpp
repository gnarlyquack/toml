#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_nested_inline_table)
{
    const string toml{"a = [ { b = {} } ]\n", 19};

    const Table expected{{ "a", new ArrayValue({new TableValue({{ "b", new TableValue({}) },}),}) },};

    assert_parsed(toml, expected);
}
