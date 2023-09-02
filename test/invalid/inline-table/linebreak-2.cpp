#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_linebreak_2)
{
    const string toml{"t = {a=1,\nb=2}\n", 15};

    const vector<Error> expected{{ 1, 10, "Unterminated inline table." },{ 2, 3, "Invalid value for decimal integer: 2}." },};

    assert_errors(toml, expected);
}
