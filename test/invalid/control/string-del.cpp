#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_string_del)
{
    const string toml{"string-del = \"null\177\"\n", 21};

    const vector<Error> expected{{ 1, 19, "Unicode character U+007F not allowed in string." },};

    assert_errors(toml, expected);
}
