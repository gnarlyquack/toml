#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_bool_tests, test_almost_false_with_extra)
{
    const string toml{"a = falsify\n", 12};

    const vector<Error> expected{{ 1, 5, "Invalid value: falsify" },};

    assert_errors(toml, expected);
}
