#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_literal_multiline_quotes_2)
{
    const string toml{"a = '''15 apostrophes: ''''''''''''''''''\n", 42};

    const vector<Error> expected{{ 1, 29, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}