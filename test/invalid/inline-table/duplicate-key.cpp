#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_duplicate_key)
{
    const string toml{"# Duplicate keys within an inline table are invalid\na={b=1, b=2}\n", 65};

    const vector<Error> expected{{ 2, 9, "Key 'b' has already been defined on line 2, character 4." },};

    assert_errors(toml, expected);
}
