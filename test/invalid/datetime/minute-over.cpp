#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_minute_over)
{
    const string toml{"# time-minute     = 2DIGIT  ; 00-59\nd = 2006-01-01T00:60:00-00:00\n", 66};

    const vector<Error> expected{{ 2, 19, "Invalid value for minute: 60." },};

    assert_errors(toml, expected);
}
