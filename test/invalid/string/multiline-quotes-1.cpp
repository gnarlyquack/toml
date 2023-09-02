#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_multiline_quotes_1)
{
    const string toml{"a = \"\"\"6 quotes: \"\"\"\"\"\"\n", 24};

    const vector<Error> expected{{ 1, 23, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
