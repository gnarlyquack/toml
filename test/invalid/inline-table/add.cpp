#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_add)
{
    const string toml{"a={}\n# Inline tables are immutable and can't be extended\n[a.b]\n", 63};

    const vector<Error> expected{{ 3, 2, "Key 'a' has already been defined on line 1, character 1." },};

    assert_errors(toml, expected);
}
