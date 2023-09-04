#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_array_tests, test_text_after_array_entries)
{
    const string toml{"array = [\n  \"Is there life after an array separator?\", No\n  \"Entry\"\n]\n", 70};

    const vector<Error> expected{{ 2, 46, "Invalid value: No" },{ 3, 3, "Missing ',' between array values." },};

    assert_errors(toml, expected);
}