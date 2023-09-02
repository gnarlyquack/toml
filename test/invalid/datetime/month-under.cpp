#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_month_under)
{
    const string toml{"# date-month      = 2DIGIT  ; 01-12\nd = 2007-00-01T00:00:00-00:00\n", 66};

    const vector<Error> expected{{ 2, 10, "Invalid value for month: 00." },};

    assert_errors(toml, expected);
}
