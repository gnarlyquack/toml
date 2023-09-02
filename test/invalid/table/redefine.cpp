#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_redefine)
{
    const string toml{"# Define b as int, and try to use it as a table: error\n[a]\nb = 1\n\n[a.b]\nc = 2\n", 78};

    const vector<Error> expected{{ 5, 4, "Key 'b' has already been defined on line 3, character 1." },};

    assert_errors(toml, expected);
}
