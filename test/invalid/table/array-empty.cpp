#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_array_empty)
{
    const string toml{"[[]]\nname = \"Born to Run\"\n", 26};

    const vector<Error> expected{{ 1, 3, "Missing key." },};

    assert_errors(toml, expected);
}
