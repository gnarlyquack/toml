#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_whitespace)
{
    const string toml{"[invalid key]\n", 14};

    const vector<Error> expected{
        { 1, 10, "Missing closing ']' for table header." },
        { 1, 10, "Expected the end of the line but got: key]" },
    };

    assert_errors(toml, expected);
}
