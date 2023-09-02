#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_invalid_bin)
{
    const string toml{"invalid-bin = 0b0012\n", 21};

    const vector<Error> expected{{ 1, 15, "Invalid value for binary integer: 0b0012." },};

    assert_errors(toml, expected);
}
