#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_double_comma)
{
    const string toml{"t = {x=3,,y=4}\n", 15};

    const vector<Error> expected{{ 1, 10, "Missing value for inline table." },};

    assert_errors(toml, expected);
}
