#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_encoding_tests, test_bad_utf8_in_string)
{
    const string toml{"# The following line contains an invalid UTF-8 sequence.\nbad = \"\303\"\n", 67};

    const vector<Error> expected{{ 0, 0, "" }};

    assert_errors(toml, expected);
}
