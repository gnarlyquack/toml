#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_multiline_bad_escape_3)
{
    const string toml{"# \\<Space> is not a valid escape.\nk = \"\"\"t\\ \"\"\"\n\n", 49};

    const vector<Error> expected{{ 2, 11, "Invalid escape: ' '." },};

    assert_errors(toml, expected);
}
