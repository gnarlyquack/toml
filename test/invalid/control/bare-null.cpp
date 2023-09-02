#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_bare_null)
{
    const string toml{"bare-null = \"some value\" \000\n", 27};

    const vector<Error> expected{{ 1, 26, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
