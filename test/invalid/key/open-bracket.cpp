#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_open_bracket)
{
    const string toml{"[abc = 1\n", 9};

    const vector<Error> expected{{ 1, 2, "Invalid key: abc = 1." },{ 1, 9, "Missing closing ']' for table." },};

    assert_errors(toml, expected);
}
