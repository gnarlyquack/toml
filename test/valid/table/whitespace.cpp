#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_whitespace)
{
    const string toml{"[\"valid key\"]\n", 14};

    const Table expected{{ "valid key", new TableValue({}) },};

    assert_parsed(toml, expected);
}
