#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_array_missing_bracket)
{
    const string toml{"[[albums]\nname = \"Born to Run\"\n", 31};

    const vector<Error> expected{{ 1, 9, "Missing closing ']' for array of tables." },};

    assert_errors(toml, expected);
}
