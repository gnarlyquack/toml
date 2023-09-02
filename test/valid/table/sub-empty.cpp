#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_sub_empty)
{
    const string toml{"[a]\n[a.b]\n", 10};

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({}) },}) },};

    assert_parsed(toml, expected);
}
