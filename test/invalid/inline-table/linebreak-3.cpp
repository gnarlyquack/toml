#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_linebreak_3)
{
    const string toml{"t = {a=1\n,b=2}\n", 15};

    const vector<Error> expected{{ 1, 9, "Unterminated inline table." },{ 2, 1, "Invalid key: ,b." },{ 2, 4, "Invalid value for decimal integer: 2}." },};

    assert_errors(toml, expected);
}