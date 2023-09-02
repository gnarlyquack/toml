#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_dotted_redefine_table)
{
    const string toml{"# Defined a.b as int\na.b = 1\n# Tries to access it as table: error\na.b.c = 2\n", 76};

    const vector<Error> expected{{ 4, 3, "Cannot add keys to 'b' because it has been defined as an integer on line 2, character 3." },};

    assert_errors(toml, expected);
}
