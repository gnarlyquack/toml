#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_bad_concat)
{
    const string toml{"no_concat = \"first\" \"second\"\n", 29};

    const vector<Error> expected{{ 1, 21, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
