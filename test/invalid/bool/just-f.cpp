#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_bool_tests, test_just_f)
{
    const string toml{"a = f\n", 6};

    const vector<Error> expected{{ 1, 5, "Invalid value: f." },};

    assert_errors(toml, expected);
}
