#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_bad_uni_esc_3)
{
    const string toml{"str = \"val\\U0000000\"\n\n", 22};

    const vector<Error> expected{{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },};

    assert_errors(toml, expected);
}