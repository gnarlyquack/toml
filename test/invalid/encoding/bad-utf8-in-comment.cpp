#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_encoding_tests, test_bad_utf8_in_comment)
{
    const string toml{"# \303\n", 4};

    const vector<Error> expected{{ 0, 0, "" }};

    assert_errors(toml, expected);
}
