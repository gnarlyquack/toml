#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_bare_vertical_tab)
{
    const string toml{"bare-vertical-tab = \v\n", 22};

    const vector<Error> expected{{ 1, 21, "Invalid value: \v." },};

    assert_errors(toml, expected);
}
