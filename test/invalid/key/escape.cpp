#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_escape)
{
    const string toml{"\\u00c0 = \"latin capital letter A with grave\"\n", 45};

    const vector<Error> expected{{ 1, 1, "Invalid key: \\u00c0" },};

    assert_errors(toml, expected);
}
