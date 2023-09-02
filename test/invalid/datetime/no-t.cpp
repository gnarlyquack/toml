#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_no_t)
{
    const string toml{"# No \"t\" or \"T\" between the date and time.\nno-t = 1987-07-0517:45:00Z\n", 70};

    const vector<Error> expected{{ 2, 16, "Invalid value for day: 0517:45:00Z." },};

    assert_errors(toml, expected);
}
