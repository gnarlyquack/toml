#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_positive_oct)
{
    const string toml{"positive-oct = +0o99\n", 21};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in octal integer: 0o99." },{ 1, 17, "Invalid value for octal integer: 0o99." },};

    assert_errors(toml, expected);
}
