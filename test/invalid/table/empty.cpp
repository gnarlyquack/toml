#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_empty)
{
    const string toml{"[]\n", 3};

    const vector<Error> expected{{ 1, 2, "Missing key." },};

    assert_errors(toml, expected);
}
