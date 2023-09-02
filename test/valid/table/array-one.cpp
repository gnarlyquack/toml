#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_array_one)
{
    const string toml{"[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n", 58};

    const Table expected{{ "people", new ArrayValue({new TableValue({{ "first_name", new StringValue("Bruce") },{ "last_name", new StringValue("Springsteen") },}),}) },};

    assert_parsed(toml, expected);
}
