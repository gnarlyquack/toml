#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_single_open_bracket)
{
    const string toml{"[\n", 2};

    const vector<Error> expected{{ 1, 2, "Missing key." },{ 1, 2, "Missing closing ']' for table." },};

    assert_errors(toml, expected);
}