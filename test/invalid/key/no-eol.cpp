#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_no_eol)
{
    const string toml{"a = 1 b = 2\n", 12};

    const vector<Error> expected{{ 1, 7, "Expected the end of the line but got: b = 2" },};

    assert_errors(toml, expected);
}
