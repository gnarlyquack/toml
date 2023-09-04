#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_open_bracket)
{
    const string toml{"[abc = 1\n", 9};

    const vector<Error> expected{
        { 1, 6, "Missing closing ']' for table header." },
        { 1, 6, "Expected the end of the line but got: = 1" },
    };

    assert_errors(toml, expected);
}
