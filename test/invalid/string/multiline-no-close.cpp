#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_multiline_no_close)
{
    const string toml{"invalid = \"\"\"\n    this will fail\n", 33};

    const vector<Error> expected{{ 3, 1, "Unterminated string." },};

    assert_errors(toml, expected);
}
