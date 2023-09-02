#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_capital_oct)
{
    const string toml{"capital-oct = 0O0\n", 18};

    const vector<Error> expected{{ 1, 15, "Invalid value for decimal integer: 0O0." },};

    assert_errors(toml, expected);
}
