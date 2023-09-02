#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_trailing_point)
{
    const string toml{"trailing-point = 1.\n", 20};

    const vector<Error> expected{{ 1, 20, "Missing fractional part of decimal number." },};

    assert_errors(toml, expected);
}
