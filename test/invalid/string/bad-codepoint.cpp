#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_bad_codepoint)
{
    const string toml{"invalid-codepoint = \"This string contains a non scalar unicode codepoint \\uD801\"\n", 81};

    const vector<Error> expected{{ 1, 1, "Invalid Unicode character U+D801." },};

    assert_errors(toml, expected);
}
