#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_duplicate_key_dotted_table2)
{
    const string toml{"[fruit]\napple.taste.sweet = true\n\n[fruit.apple.taste] # INVALID\n", 64};

    const vector<Error> expected{{ 4, 14, "Key 'taste' has already been defined on line 2, character 7." },};

    assert_errors(toml, expected);
}
