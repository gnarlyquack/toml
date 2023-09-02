#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_double_sign_plus)
{
    const string toml{"double-sign-plus = ++99\n", 24};

    const vector<Error> expected{{ 1, 21, "Invalid value: +99." },};

    assert_errors(toml, expected);
}
