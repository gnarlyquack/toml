#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_no_secs)
{
    const string toml{"# No seconds in time.\nno-secs = 1987-07-05T17:45Z\n", 50};

    const vector<Error> expected{{ 2, 27, "Missing ':' between minute and second." },{ 2, 27, "Missing value for second." },};

    assert_errors(toml, expected);
}
