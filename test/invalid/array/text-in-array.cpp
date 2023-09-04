#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_array_tests, test_text_in_array)
{
    const string toml{"array = [\n  \"Entry 1\",\n  I don't belong,\n  \"Entry 2\",\n]\n", 56};

    const vector<Error> expected{{ 3, 3, "Invalid value: I don't belong" },};

    assert_errors(toml, expected);
}
