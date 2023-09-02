#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_after_value)
{
    const string toml{"first = \"Tom\" last = \"Preston-Werner\" # INVALID\n", 48};

    const vector<Error> expected{{ 1, 15, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
