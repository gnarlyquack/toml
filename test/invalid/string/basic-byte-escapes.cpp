#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_basic_byte_escapes)
{
    const string toml{"answer = \"\\x33\"\n", 16};

    const vector<Error> expected{{ 1, 12, "Invalid escape: 'x'." },};

    assert_errors(toml, expected);
}
