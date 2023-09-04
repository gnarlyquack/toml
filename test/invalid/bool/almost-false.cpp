#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_bool_tests, test_almost_false)
{
    const string toml{"a = fals\n", 9};

    const vector<Error> expected{{ 1, 5, "Invalid value: fals." },};

    assert_errors(toml, expected);
}