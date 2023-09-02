#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_second_over)
{
    const string toml{"# time-second     = 2DIGIT  ; 00-58, 00-59, 00-60 based on leap second\n#                           ; rules\nd = 2006-01-01T00:00:61-00:00\n", 137};

    const vector<Error> expected{{ 3, 22, "Invalid value for second: 61." },};

    assert_errors(toml, expected);
}
