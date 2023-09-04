#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_bool_tests, test_starting_same_true)
{
    const string toml{"a = truer\n", 10};

    const vector<Error> expected{{ 1, 5, "Invalid value: truer" },};

    assert_errors(toml, expected);
}
