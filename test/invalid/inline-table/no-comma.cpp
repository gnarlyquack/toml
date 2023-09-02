#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_no_comma)
{
    const string toml{"t = {x = 3 y = 4}\n", 18};

    const vector<Error> expected{{ 1, 12, "Missing ',' between inline table values." },};

    assert_errors(toml, expected);
}
