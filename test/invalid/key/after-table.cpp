#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_after_table)
{
    const string toml{"[error] this = \"should not be here\"\n", 36};

    const vector<Error> expected{{ 1, 9, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
