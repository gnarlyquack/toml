#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_start_bracket)
{
    const string toml{"[a]\n[xyz = 5\n[b]\n", 17};

    const vector<Error> expected{{ 2, 2, "Invalid key: xyz = 5." },{ 2, 9, "Missing closing ']' for table." },};

    assert_errors(toml, expected);
}
