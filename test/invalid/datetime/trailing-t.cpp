#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_trailing_t)
{
    const string toml{"# Date cannot end with trailing T\nd = 2006-01-30T\n", 50};

    const vector<Error> expected{{ 2, 16, "Missing time." }};

    assert_errors(toml, expected);
}
