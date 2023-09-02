#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_double_quote_escape)
{
    const string toml{"test = \"\\\"one\\\"\"\n", 17};

    const Table expected{{ "test", new StringValue("\"one\"") },};

    assert_parsed(toml, expected);
}
