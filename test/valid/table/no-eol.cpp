#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_no_eol)
{
    const string toml{"[table]\n", 8};

    const Table expected{{ "table", new TableValue({}) },};

    assert_parsed(toml, expected);
}