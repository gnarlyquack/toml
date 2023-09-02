#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_no_close)
{
    const string toml{"no-ending-quote = \"One time, at band camp\n", 42};

    const vector<Error> expected{{ 1, 42, "Unterminated string." },};

    assert_errors(toml, expected);
}
