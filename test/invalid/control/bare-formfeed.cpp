#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_bare_formfeed)
{
    const string toml{"bare-formfeed = \f\n", 18};

    const vector<Error> expected{{ 1, 17, "Unicode codepoint U+000C is not allowed." },};

    assert_errors(toml, expected);
}