#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_nan_incomplete_2)
{
    const string toml{"nan-incomplete-2 = +na\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid value: +na" },};

    assert_errors(toml, expected);
}
