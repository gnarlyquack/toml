#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_mday_over)
{
    const string toml{"# date-mday       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on\n#                           ; month/year\nd = 2006-01-32T00:00:00-00:00\n", 137};

    const vector<Error> expected{{ 3, 13, "Day must be between 01 and 31 inclusive but value was: 32" },};

    assert_errors(toml, expected);
}
