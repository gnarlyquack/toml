#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_bare_formfeed)
{
    const string toml{"bare-formfeed = \f\n", 18};

    const vector<Error> expected{{ 1, 17, "Invalid value: \f." },};

    assert_errors(toml, expected);
}
