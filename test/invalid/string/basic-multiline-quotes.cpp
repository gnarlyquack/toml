#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_basic_multiline_quotes)
{
    const string toml{"str5 = \"\"\"Here are three quotation marks: \"\"\".\"\"\"\n", 50};

    const vector<Error> expected{{ 1, 46, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}