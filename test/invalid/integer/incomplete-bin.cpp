#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_incomplete_bin)
{
    const string toml{"incomplete-bin = 0b\n", 20};

    const vector<Error> expected{{ 1, 20, "Missing value for binary integer." },};

    assert_errors(toml, expected);
}
