#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_double_sign_nex)
{
    const string toml{"double-sign-nex = --99\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid value: -99." },};

    assert_errors(toml, expected);
}
