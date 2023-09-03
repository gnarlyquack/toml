#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_time_no_leads)
{
    const string toml{"# Leading 0 is always required.\nd = 1:32:00\n", 44};

    const vector<Error> expected{{ 2, 5, "Hour must be two digits." },};

    assert_errors(toml, expected);
}
