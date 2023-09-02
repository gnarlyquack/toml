#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_table_array_string_backslash)
{
    const string toml{"foo = [ { bar=\"\\\"{{baz}}\\\"\"} ]\n", 31};

    const Table expected{{ "foo", new ArrayValue({new TableValue({{ "bar", new StringValue("\"{{baz}}\"") },}),}) },};

    assert_parsed(toml, expected);
}
