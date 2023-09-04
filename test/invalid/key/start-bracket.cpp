#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_start_bracket)
{
    const string toml{"[a]\n[xyz = 5\n[b]\n", 17};

    const vector<Error> expected{
        { 2, 6, "Missing closing ']' for table header." },
        { 2, 6, "Expected the end of the line but got: = 5" },
    };

    assert_errors(toml, expected);
}
