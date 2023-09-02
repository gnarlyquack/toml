#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_add)
{
    const string toml{"a={}\n# Inline tables are immutable and can't be extended\n[a.b]\n", 63};

    const vector<Error> expected{{ 3, 2, "Cannot add keys to 'a' because it has been defined as an inline table on line 1, character 1." },};

    assert_errors(toml, expected);
}
