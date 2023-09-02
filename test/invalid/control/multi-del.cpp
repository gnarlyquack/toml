#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_multi_del)
{
    const string toml{"multi-del = \"\"\"null\177\"\"\"\n", 24};

    const vector<Error> expected{{ 1, 20, "Unicode character U+007F not allowed in string." },};

    assert_errors(toml, expected);
}
