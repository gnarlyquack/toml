#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_empty)
{
    const string toml{"[a]\n", 4};

    const Table expected{{ "a", new TableValue({}) },};

    assert_parsed(toml, expected);
}
