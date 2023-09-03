#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_month_over)
{
    const string toml{"# date-month      = 2DIGIT  ; 01-12\nd = 2006-13-01T00:00:00-00:00\n", 66};

    const vector<Error> expected{{ 2, 10, "Month must be between 01 and 12 inclusive but value was: 13" },};

    assert_errors(toml, expected);
}
