#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_space)
{
    const string toml{"a b = 1\n", 8};

    const vector<Error> expected{{ 1, 1, "Invalid key: a b." },};

    assert_errors(toml, expected);
}
