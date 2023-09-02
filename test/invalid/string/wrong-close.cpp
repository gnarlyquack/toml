#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_wrong_close)
{
    const string toml{"bad-ending-quote = \"double and single'\n", 39};

    const vector<Error> expected{{ 1, 39, "Unterminated string." },};

    assert_errors(toml, expected);
}
