#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_invalid_oct)
{
    const string toml{"invalid-oct = 0o778\n", 20};

    const vector<Error> expected{{ 1, 15, "Invalid value for octal integer: 0o778." },};

    assert_errors(toml, expected);
}
