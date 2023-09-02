#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_empty)
{
    const string toml{"t = {,}\n", 8};

    const vector<Error> expected{{ 1, 6, "Missing value for inline table." },{ 1, 8, "Trailing ',' not allowed in inline table." },};

    assert_errors(toml, expected);
}
