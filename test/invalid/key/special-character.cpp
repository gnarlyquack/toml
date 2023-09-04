#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_special_character)
{
    const string toml{"\316\274 = \"greek small letter mu\"\n", 29};

    const vector<Error> expected{{ 1, 1, "Invalid key: μ" },};

    assert_errors(toml, expected);
}
