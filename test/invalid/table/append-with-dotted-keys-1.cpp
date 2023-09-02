#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_append_with_dotted_keys_1)
{
    const string toml{"# First a.b.c defines a table: a.b.c = {z=9}\n#\n# Then we define a.b.c.t = \"str\" to add a str to the above table, making it:\n#\n#   a.b.c = {z=9, t=\"...\"}\n#\n# While this makes sense, logically, it was decided this is not valid TOML as\n# it's too confusing/convoluted.\n# \n# See: https://github.com/toml-lang/toml/issues/846\n#      https://github.com/toml-lang/toml/pull/859\n\n[a.b.c]\n  z = 9\n\n[a]\n  b.c.t = \"Using dotted keys to add to [a.b.c] after explicitly defining it above is not allowed\"\n", 491};

    const vector<Error> expected{{ 17, 5, "Key 'c' has already been defined on line 13, character 6." },};

    assert_errors(toml, expected);
}
