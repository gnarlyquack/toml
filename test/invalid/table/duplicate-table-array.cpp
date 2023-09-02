#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_duplicate_table_array)
{
    const string toml{"[tbl]\n[[tbl]]\n", 14};

    const vector<Error> expected{{ 2, 3, "Key 'tbl' has already been defined on line 1, character 2." },};

    assert_errors(toml, expected);
}
