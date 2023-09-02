#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_inf_incomplete_3)
{
    const string toml{"inf-incomplete-3 = -in\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid value: -in" },};

    assert_errors(toml, expected);
}
