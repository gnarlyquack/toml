#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_array_tests, test_tables_2)
{
    const string toml{"# INVALID TOML DOC\n[[fruit]]\n  name = \"apple\"\n\n  [[fruit.variety]]\n    name = \"red delicious\"\n\n  # This table conflicts with the previous table\n  [fruit.variety]\n    name = \"granny smith\"\n", 188};

    const vector<Error> expected{{ 9, 10, "Key 'variety' has already been defined on line 5, character 11." },};

    assert_errors(toml, expected);
}
