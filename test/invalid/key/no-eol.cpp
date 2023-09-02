#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_no_eol)
{
    const string toml{"a = 1 b = 2\n", 12};

    const vector<Error> expected{{ 1, 7, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
